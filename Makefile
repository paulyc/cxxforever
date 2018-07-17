# Default:
#   just run make in any subdirectory containing a Makefile.
# To recurse this behavior into a subdirectory, 
#   include this Makefile from the subdirectory's Makefile.
# To override this behavior, 
#   include this Makefile from the subdirectory's Makefile
#   and redefine the 'all' rule.
# To override this behavior and continue recursion,
#   include this Makefile from the subdirectory's Makefile
#   and redefine the 'all' rule with the 'subdirs' rule as a dependency.

SUBDIRS = $(sort $(dir $(wildcard */Makefile)))

.PHONY: subdirs $(SUBDIRS)

all: subdirs

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

all: cxxforever

SHARED_LIB_EXTENSION := dylib
SHARED_LIB_VERSION := 0.0

clean:
	rm -f libtwilio++.${SHARED_LIB_EXTENSION}.${SHARED_LIB_VERSION} cxxforever

libtwilio.shared: subdirs
	$(LD) -shared -o libtwilio++.${SHARED_LIB_EXTENSION}.${SHARED_LIB_VERSION} src/.*.lo

cxxforever: libtwilio.shared
	$(CXX) -o c++forever src/main.cpp -ltwilio++
