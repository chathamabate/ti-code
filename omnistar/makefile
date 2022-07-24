
# ----------------------------
# Makefile Options
# ----------------------------

NAME = OSTR
ICON = icon.png
DESCRIPTION = "OmniStar"
COMPRESSED := YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz -I.. 
CXXFLAGS = -Wall -Wextra -Oz -I..

# This Actually Works!!!
HAS_PRINTF := NO

# # Add it sources boy....
EXTRA_C_SOURCES = $(wildcard ../cutil/*.c)
EXTRA_CXX_SOURCES = $(wildcard ../cxxutil/*.cpp)
EXTRA_HEADERS = $(wildcard ../cutil/*.h ../cxxutil/*.h)


$(info [${EXTRA_USERHEADERS}])
# ----------------------------


include $(shell cedev-config --makefile)
