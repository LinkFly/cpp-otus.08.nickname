#include "share.h"

#define BOOST_TEST_MODULE allocator_test_module

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <functional>
#include <ctime>
#include <sstream>

#include "nickname.h"

using std::string;
using std::cout;
using std::endl;
using std::clock;

bool call_test(string name, std::function<bool(void)> fntest) {
	cout << "------------------------------\n";
	cout << endl << name << ":\n";

	auto startTime = clock();
	bool res = fntest();
	auto endTime = clock();

	cout << "TIME: " << endTime - startTime << "ms" << endl;
	cout << "------------------------------\n";
	return res;
}

bool trivial_test_radix_trie() {
	return call_test(__PRETTY_FUNCTION__, []() {
		std::ostringstream sout, sout2;
		RadixTrie<> rtree;
		rtree.append("mark");
		rtree.append("mast");
		rtree.printTree(sout);
		rtree.print(sout2);
		auto res1 = sout.str();
		auto res2 = sout2.str();
		string waitRes1 = "ma\n\trk\n\t\st\n";
		string waitRes2 = "mark mar\nmast mas\n";
		return res1 == waitRes1 && res2 == waitRes2;
	});
}

//struct Init {
//	Init(std::function<void()> init_func) {
//		init_func();
//	}
//};
//#define INIT(init_func) struct Init init(init_func);


BOOST_AUTO_TEST_SUITE(allocator_test_suite)
//INIT(init_base_fixtures)

BOOST_AUTO_TEST_CASE(test_of_nickname)
{
	BOOST_CHECK(trivial_test_radix_trie());
}

BOOST_AUTO_TEST_SUITE_END()
