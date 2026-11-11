#include "BookSearch.h"

#include <Epub.h>
#include <Epub/VisibleTextUtils.h>
#include <Epub/htmlEntities.h>
#include <Logging.h>
#include <strings.h>

#include <climits>
#include <cstring>

namespace {

// Decode one UTF-8 codepoint, advancing p (invalid bytes come back as themselves).
uint32_t nextCodepoint(const unsigned char*& p, const unsigned char* end) {
  const unsigned char c = *p++;
  if (c < 0x80) return c;
  int extra = (c & 0xE0) == 0xC0 ? 1 : (c & 0xF0) == 0xE0 ? 2 : (c & 0xF8) == 0xF0 ? 3 : 0;
  uint32_t cp = extra == 1 ? (c & 0x1F) : extra == 2 ? (c & 0x0F) : (c & 0x07);
  if (extra == 0) return c;
  while (extra-- > 0 && p < end && (*p & 0xC0) == 0x80) cp = (cp << 6) | (*p++ & 0x3F);
  return cp;
}

bool isSpace(const uint32_t cp) { return cp == ' ' || cp == '\n' || cp == '\r' || cp == '\t' || cp == 0xA0; }

// Case fold for matching: ASCII and Latin-1 capitals to lower case.
uint32_t fold(const uint32_t cp) {
  if (cp >= 'A' && cp <= 'Z') return cp + 32;
  if (cp >= 0xC0 && cp <= 0xDE && cp != 0xD7) return cp + 32;
  return cp;
}

}  // namespace

bool BookSearch::begin(const std::string& query) {
  query_.clear();
  hits_.clear();
  bool lastSpace = true;
  const auto* p = reinterpret_cast<const unsigned char*>(query.data());
  const auto* end = p + query.size();
  while (p < end) {
    const uint32_t cp = nextCodepoint(p, end);
    if (isSpace(cp)) {
      if (!lastSpace) query_.push_back(' ');
      lastSpace = true;
    } else {
      query_.push_back(fold(cp));
      lastSpace = false;
    }
  }
  while (!query_.empty() && query_.back() == ' ') query_.pop_back();
  return !query_.empty();
}

void BookSearch::resetChapter() {
  window_.assign(query_.size(), 0);
  windowStart_.assign(query_.size(), 0);
  windowFill_ = 0;
  windowHead_ = 0;
  lastWasSpace_ = true;
  context_.clear();
  pendingFrom_ = hits_.size();
  pendingBytes_ = 0;
  visibleOffset_ = 0;
  insideBody_ = false;
  nonVisibleDepth_ = 0;
  parseFailed_ = false;
}

void BookSearch::searchSpine(const Epub& epub, const int spineIndex) {
  if (query_.empty() || full()) return;
  spineIndex_ = spineIndex;
  resetChapter();
  parser_ = XML_ParserCreate(nullptr);
  if (!parser_) return;
  XML_SetUserData(parser_, this);
  XML_SetElementHandler(parser_, onStart, onEnd);
  XML_SetCharacterDataHandler(parser_, onText);
  // HTML entities (&nbsp; ...) as the chapter parser expands them.
  XML_SetDefaultHandlerExpand(parser_, onDefault);
  const auto href = epub.getSpineItem(spineIndex).href;
  if (epub.readItemContentsToStream(href, *this, 4096) && !parseFailed_) {
    XML_Parse(parser_, "", 0, XML_TRUE);
  }
  destroyXmlParser(parser_);
}

size_t BookSearch::write(const uint8_t* data, const size_t size) {
  if (!parser_ || parseFailed_) return size;  // keep the stream going; nothing more to find here
  if (size > INT_MAX ||
      XML_Parse(parser_, reinterpret_cast<const char*>(data), static_cast<int>(size), XML_FALSE) == XML_STATUS_ERROR) {
    // Text after </html> and similar: keep the hits found so far.
    parseFailed_ = true;
  }
  return size;
}

void XMLCALL BookSearch::onStart(void* user, const XML_Char* name, const XML_Char**) {
  auto* self = static_cast<BookSearch*>(user);
  if (strcasecmp(name, "body") == 0) self->insideBody_ = true;
  if (self->insideBody_ && (self->nonVisibleDepth_ > 0 || VisibleTextUtils::isNonVisibleElement(name))) {
    self->nonVisibleDepth_++;
  }
}

void XMLCALL BookSearch::onEnd(void* user, const XML_Char* name) {
  auto* self = static_cast<BookSearch*>(user);
  if (self->nonVisibleDepth_ > 0) self->nonVisibleDepth_--;
  if (strcasecmp(name, "body") == 0) self->insideBody_ = false;
}

void XMLCALL BookSearch::onText(void* user, const XML_Char* s, const int len) {
  static_cast<BookSearch*>(user)->visibleText(s, len);
}

void XMLCALL BookSearch::onDefault(void* user, const XML_Char* s, const int len) {
  if (len >= 3 && s[0] == '&' && s[len - 1] == ';') {
    const char* value = lookupHtmlEntity(s, static_cast<size_t>(len));
    if (value) {
      onText(user, value, static_cast<int>(strlen(value)));
    } else {
      onText(user, s, len);  // unknown entity: kept as written, like the chapter parser
    }
  }
}

void BookSearch::visibleText(const char* s, const int len) {
  if (!insideBody_ || nonVisibleDepth_ > 0 || len <= 0) return;
  const auto* p = reinterpret_cast<const unsigned char*>(s);
  const auto* end = p + len;
  while (p < end) {
    const auto* start = p;
    const uint32_t cp = nextCodepoint(p, end);
    const uint32_t offset = visibleOffset_++;
    if (isSpace(cp)) {
      if (lastWasSpace_) continue;
      lastWasSpace_ = true;
      appendDisplay(" ", 1);
      feed(' ', offset);
    } else {
      lastWasSpace_ = false;
      appendDisplay(reinterpret_cast<const char*>(start), static_cast<size_t>(p - start));
      feed(fold(cp), offset);
    }
  }
}

void BookSearch::appendDisplay(const char* utf8, const size_t len) {
  context_.append(utf8, len);
  // Hits still collecting their trailing context.
  if (pendingFrom_ < hits_.size()) {
    for (size_t i = pendingFrom_; i < hits_.size(); ++i) hits_[i].snippet.append(utf8, len);
    pendingBytes_ += len;
    if (pendingBytes_ >= kContextBytes) {
      pendingFrom_ = hits_.size();
      pendingBytes_ = 0;
    }
  }
  if (context_.size() > 2 * kContextBytes + 64) {
    // Drop from the front on a codepoint boundary.
    size_t cut = context_.size() - (kContextBytes + 64);
    while (cut < context_.size() && (static_cast<unsigned char>(context_[cut]) & 0xC0) == 0x80) ++cut;
    context_.erase(0, cut);
  }
}

void BookSearch::feed(const uint32_t cp, const uint32_t offset) {
  const size_t n = query_.size();
  window_[windowHead_] = cp;
  windowStart_[windowHead_] = offset;
  windowHead_ = (windowHead_ + 1) % n;
  if (windowFill_ < n) ++windowFill_;
  if (windowFill_ < n || full()) return;
  // windowHead_ now points at the oldest entry.
  for (size_t i = 0; i < n; ++i) {
    if (window_[(windowHead_ + i) % n] != query_[i]) return;
  }
  Hit hit;
  hit.spineIndex = spineIndex_;
  hit.visibleOffset = windowStart_[windowHead_];
  // Context before the match: the last kContextBytes of display text before it.
  size_t keep = std::min(context_.size(), kContextBytes + 64);
  size_t from = context_.size() - keep;
  while (from < context_.size() && (static_cast<unsigned char>(context_[from]) & 0xC0) == 0x80) ++from;
  hit.snippet = context_.substr(from);
  if (pendingFrom_ >= hits_.size()) {
    pendingFrom_ = hits_.size();
    pendingBytes_ = 0;
  }
  hits_.push_back(std::move(hit));
}
