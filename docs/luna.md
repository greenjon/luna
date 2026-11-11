# Luna additions

Luna is a fork of CrossPoint Reader with a few reading comforts borrowed from
Moon+ Reader. Everything else works as in CrossPoint.

## Toolbar top bar
With **Reader Menu Style** set to **Toolbar**, opening the toolbar also shows
a bar across the top of the page with the book title, the clock and the
battery. The clock shows there even when the header clock setting is off.

Below it, once Luna has seen you read a few pages, a second line shows the
time left: "12 min left in chapter, 3 h 20 min in book". The estimate comes
from how long pages stay on screen before you turn forward (skims under 3
seconds and pauses over 10 minutes don't count), averaged over recent pages
and kept across books. The book estimate assumes the rest of the book is as
dense as the current chapter, so it moves as you read.

## Time left in the status bar
Settings → Reader → Customise Status Bar → **Title** has a fourth choice,
**Time left**: the status bar then shows "12 min left in chapter" on every
page, from the same reading pace. It stays empty until the pace is known.
EPUB books only.

## Search in book
**Search in book** (reader menu / More list, or a toolbar button or gesture
with the magnifier icon) asks for a word or phrase, then searches the whole
book chapter by chapter, listing each match with its chapter and the text
around it. Letter case doesn't matter, and any run of spaces or line breaks
counts as one space. Choose a match to jump to its page. Back stops a running
search and keeps the matches found so far; the list stops at 100 matches.
The last query is filled in next time while the book stays open. A large
book can take a few seconds per chapter on this hardware.

## Reading statistics
**Reading statistics** (in the reader menu's More list, as a toolbar button
or gesture, and under Settings → Reader) shows this book's reading time and
pages turned, the time read today, in the last 7 and 30 days and in total,
the reading pace, and the recently read books. Time counts active reading
only: the time each page stays on screen, with long pauses capped. Totals are
saved to `/.crosspoint/luna_stats.json` when a book closes or the device
sleeps (the last 40 books and 60 days are kept). Day totals need the clock.

## Text weight
Settings → Reader → **Text weight** (also in the toolbar's Text panel):
**Normal**, **Medium** or **Heavy** widens every letter's strokes by 0, 1 or
2 pixels. Heavier strokes hide the stair-steps on curves and diagonals and
read darker on e-ink. Letter spacing and page layout stay the same, so
changing it never re-paginates the book. It applies to EPUB pages with
anti-aliased (2-bit) fonts, which covers the built-in and SD-card fonts.

## Text presets
In the toolbar's Text panel, **Text presets** saves the current text
settings (font, size, line, word and letter spacing, paragraph spacing and
indent, alignment, margins, hyphenation, book styles, focus reading,
anti-aliasing and text weight) into one of three presets, and applies a
saved preset in one step, for example one for day reading and one for a
larger, heavier night setting. Presets live in
`/.crosspoint/luna_presets.json`.

## Taps and gestures
See [controls.md](controls.md): tap zones with drawn layouts, tap and swipe
switches, the left-edge brightness slide, swipe up/down actions, and toolbar
buttons.
