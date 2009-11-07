AM_CXXFLAGS = -Wall -I../include/

TESTS = \
	test-int-types \
	test-object-io \
	test-object-array \
	test-object-pool \
	test-basic-trie \
	test-ternary-trie \
	test-bit-vector \
	test-basic-succinct-bit-vector \
	test-succinct-trie \
	test-louds-trie \
	test-louds-plus-trie \
	test-da-trie \
	test-basic-completer \
	test-value-order-completer

noinst_bindir = .
noinst_bin_PROGRAMS = $(TESTS)

test_int_types_SOURCES = test-int-types.cc
test_object_io_SOURCES = test-object-io.cc
test_object_array_SOURCES = test-object-array.cc
test_object_pool_SOURCES = test-object-pool.cc
test_basic_trie_SOURCES = test-basic-trie.cc
test_ternary_trie_SOURCES = test-ternary-trie.cc
test_bit_vector_SOURCES = test-bit-vector.cc
test_basic_succinct_bit_vector_SOURCES = test-basic-succinct-bit-vector.cc
test_succinct_trie_SOURCES = test-succinct-trie.cc
test_louds_trie_SOURCES = test-louds-trie.cc
test_louds_plus_trie_SOURCES = test-louds-plus-trie.cc
test_da_trie_SOURCES = test-da-trie.cc
test_basic_completer_SOURCES = test-basic-completer.cc
test_value_order_completer_SOURCES = test-value-order-completer.cc

dist_noinst_HEADERS = test-tools.h

dist_noinst_DATA = test-keys.txt
