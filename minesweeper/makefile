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
EXTRA_C_SOURCES = $(wildcard ../cutil/*.c)
EXTRA_HEADERS = $(wildcard ../cutil/*.h)

$(info [${EXTRA_USERHEADERS}])
# ----------------------------


include $(shell cedev-config --makefile)
