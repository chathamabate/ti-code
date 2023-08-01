
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

# This is because I use snprintf.
HAS_PRINTF := NO

# Add it sources boy....
EXTRA_C_SOURCES = $(wildcard ../cutil/*.c)

# NOTE: If you wish to use a module from cxxutil, make sure to add it here.
CXX_UTIL_MODS 		= core unit

CXX_UTIL_SOURCES 	= $(foreach mod,$(CXX_UTIL_MODS),$(wildcard ../cxxutil/$(mod)/*.cpp)) 
CXX_UTIL_HEADERS	= $(foreach mod,$(CXX_UTIL_MODS),$(wildcard ../cxxutil/$(mod)/*.h)) 

EXTRA_CXX_SOURCES = $(CXX_UTIL_SOURCES)
EXTRA_C_SOURCES = $(wildcard ../cutil/*.c)

EXTRA_HEADERS = $(wildcard ../cutil/*.h) $(CXX_UTIL_HEADERS)

$(info [${EXTRA_USERHEADERS}])
# ----------------------------


include $(shell cedev-config --makefile)
