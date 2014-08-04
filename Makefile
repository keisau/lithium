TARGETS				= hash_file	\
					  hash_test \
					  hashmap_test \
					  radix_tree_test

CXXFLAGS			= -I.

CXX					= c++

all: $(TARGETS)

clean:
	@rm -rf $(TARGETS) *.o

%: %.c
	$(CXX) $(CXXFLAGS) $< -o $@
