# ----------------------------
# Makefile Options
# ----------------------------

NAME = USB
ICON = icon.png
DESCRIPTION = "TI-USB"
COMPRESSED := YES
ARCHIVED = NO

CFLAGS = -Wall -Wextra -Oz -I.. 
CXXFLAGS = -Wall -Wextra -Oz -I..

HAS_PRINTF := YES

# Core Modules without testing code.
CXX_UTIL_CORE_MODS			= core unit

# Non Core modules which can be tested.
CXX_UTIL_NON_CORE_MODS		= gui unitapp app data

CXX_UTIL_MODS				= $(CXX_UTIL_CORE_MODS) $(CXX_UTIL_NON_CORE_MODS)

CXX_UTIL_SOURCES 		= $(foreach mod,$(CXX_UTIL_MODS),$(wildcard ../cxxutil/$(mod)/*.cpp)) 
CXX_UTIL_TEST_SOURCES	= $(foreach mod,$(CXX_UTIL_NON_CORE_MODS),$(wildcard ../cxxutil/$(mod)/test/*.cpp)) 

CXX_UTIL_HEADERS		= $(foreach mod,$(CXX_UTIL_MODS),$(wildcard ../cxxutil/$(mod)/*.h)) 
CXX_UTIL_TEST_HEADERS	= $(foreach mod,$(CXX_UTIL_NON_CORE_MODS),$(wildcard ../cxxutil/$(mod)/test/*.h)) 

EXTRA_CXX_SOURCES = $(CXX_UTIL_SOURCES) $(CXX_UTIL_TEST_SOURCES)
EXTRA_C_SOURCES = $(wildcard ../cutil/*.c)

EXTRA_HEADERS = $(wildcard ../cutil/*.h) $(CXX_UTIL_HEADERS) $(CXX_UTIL_TEST_HEADERS)

$(info [${EXTRA_USERHEADERS}])
# ----------------------------


include $(shell cedev-config --makefile)
