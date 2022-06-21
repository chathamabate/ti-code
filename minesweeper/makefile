# ----------------------------
# Makefile Options
# ----------------------------

NAME = DEMO
ICON = icon.png
DESCRIPTION = "CE C Toolchain Demo"
COMPRESSED = NO
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz -I.. 
CXXFLAGS = -Wall -Wextra -Oz -I..

# This Actually Works!!!
HAS_PRINTF := NO

# Big time space saver.
COMPRESSED := YES

# # Add it sources boy....
EXTRA_CSOURCES = $(wildcard ../cutil/*.c)
EXTRA_USERHEADERS = $(wildcard ../cutil/*.h)

$(info [${EXTRA_USERHEADERS}])
# ----------------------------


include $(shell cedev-config --makefile)
