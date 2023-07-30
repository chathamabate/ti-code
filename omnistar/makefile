
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

CXX_UTIL_MODS 		= core
CXX_UTIL_SOURCES 	= $(foreach mod,$(CXX_UTIL_MODS),$(wildcard ../cxxutil/$(mod)/*.cpp)) 
CXX_UTIL_HEADERS	= $(foreach mod,$(CXX_UTIL_MODS),$(wildcard ../cxxutil/$(mod)/*.h)) 

EXTRA_CXX_SOURCES = $(CXX_UTIL_SOURCES)
EXTRA_C_SOURCES = $(wildcard ../cutil/*.c)

EXTRA_HEADERS = $(wildcard ../cutil/*.h) $(CXX_UTIL_HEADERS)

$(info [${EXTRA_USERHEADERS}])
# ----------------------------


include $(shell cedev-config --makefile)
