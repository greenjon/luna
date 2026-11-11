#pragma once

#include <Print.h>
#include <XmlParserUtils.h>

#include <cstdint>
#include <string>
#include <vector>

class Epub;

// Full-text search in an EPUB, one spine item at a time (Luna). Each item's
// XHTML streams from the ZIP through expat; the visible text is counted
// exactly as ChapterHtmlSlimParser counts it (codepoints of body character
// data outside head/style/script/title/rp, HTML entities expanded), so a hit's
// offset is a "visible text offset" the reader can jump to like a bookmark.
// Matching ignores letter case (ASCII and Latin-1 letters) and treats any run
// of whitespace as one space.
class BookSearch final : public Print {
 public:
  struct Hit {
    int spineIndex = 0;
    uint32_t visibleOffset = 0;
    std::string snippet;  // the match with some text either side
  };

  static constexpr size_t kMaxHits = 100;

  // False when the query has no searchable characters.
  bool begin(const std::string& query);
  // Search one spine item, appending to hits() (up to kMaxHits).
  void searchSpine(const Epub& epub, int spineIndex);
  const std::vector<Hit>& hits() const { return hits_; }
  bool full() const { return hits_.size() >= kMaxHits; }

  size_t write(uint8_t c) override { return write(&c, 1); }
  size_t write(const uint8_t* data, size_t size) override;

 private:
  static constexpr size_t kContextBytes = 60;  // snippet text kept before / after a match

  std::vector<uint32_t> query_;        // folded codepoints
  std::vector<uint32_t> window_;       // last query_.size() folded codepoints
  std::vector<uint32_t> windowStart_;  // their visible offsets
  size_t windowFill_ = 0;
  size_t windowHead_ = 0;  // ring position of the next write
  bool lastWasSpace_ = true;

  std::vector<Hit> hits_;
  std::string context_;      // recent display text (collapsed whitespace)
  size_t pendingFrom_ = 0;   // hits_ from here still collect text after the match
  size_t pendingBytes_ = 0;  // ... this many bytes so far

  XML_Parser parser_ = nullptr;
  int spineIndex_ = 0;
  uint32_t visibleOffset_ = 0;
  bool insideBody_ = false;
  int nonVisibleDepth_ = 0;
  bool parseFailed_ = false;

  void resetChapter();
  void visibleText(const char* s, int len);
  void feed(uint32_t cp, uint32_t offset);
  void appendDisplay(const char* utf8, size_t len);

  static void XMLCALL onStart(void* user, const XML_Char* name, const XML_Char** atts);
  static void XMLCALL onEnd(void* user, const XML_Char* name);
  static void XMLCALL onText(void* user, const XML_Char* s, int len);
  static void XMLCALL onDefault(void* user, const XML_Char* s, int len);
};
