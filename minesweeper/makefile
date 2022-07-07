# ----------------------------
# Makefile Options
# ----------------------------

NAME = MS
ICON = icon.png
DESCRIPTION = "Minesweeper!"
COMPRESSED := YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz -I.. 
CXXFLAGS = -Wall -Wextra -Oz -I..

# This Actually Works!!!
HAS_PRINTF := NO

# # Add it sources boy....
EXTRA_CSOURCES = $(wildcard ../cutil/*.c)
EXTRA_USERHEADERS = $(wildcard ../cutil/*.h)

$(info [${EXTRA_USERHEADERS}])
# ----------------------------


include $(shell cedev-config --makefile)
