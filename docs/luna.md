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

## Text weight
Settings → Reader → **Text weight** (also in the toolbar's Text panel):
**Normal**, **Medium** or **Heavy** widens every letter's strokes by 0, 1 or
2 pixels. Heavier strokes hide the stair-steps on curves and diagonals and
read darker on e-ink. Letter spacing and page layout stay the same, so
changing it never re-paginates the book. It applies to EPUB pages with
anti-aliased (2-bit) fonts, which covers the built-in and SD-card fonts.

## Taps and gestures
See [controls.md](controls.md): tap zones with drawn layouts, tap and swipe
switches, the left-edge brightness slide, swipe up/down actions, and toolbar
buttons.
