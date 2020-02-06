#include "share.h"

#define BOOST_TEST_MODULE allocator_test_module

#include <boost/test/unit_test.hpp>
#include <iostream>
#include <functional>
#include <ctime>
#include <sstream>
#include <map>
#include <fstream>
#include <vector>
#include <locale>
#include <codecvt>
#include <cstdlib>

#include "nickname.h"
#include "tests-utils.h"

using std::wstring;
using std::string;
using std::cout;
using std::endl;
using std::clock;

// Utils
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

void addTestNames(RadixTrie& rtree) {
	auto names = readTestNames();
	for (auto name : names) {
		rtree.append(name);
	}
}
//// end Utils

bool trivial_test_radix_trie() {
	return call_test(__PRETTY_FUNCTION__, []() {
		std::wostringstream sout, sout2;
		RadixTrie rtree;
		rtree.append(L"mark");
		rtree.append(L"mast");
		rtree.printTree(sout);
		rtree.print(sout2);
		auto res1 = sout.str();
		auto res2 = sout2.str();
		wstring waitRes1 = L"ma\n\trk$\n\tst$\n";
		wstring waitRes2 = L"mark mar\nmast mas\n";
		return res1 == waitRes1 && res2 == waitRes2;
	});
}

bool utf8_test_radix_trie() {
/*
Данил - Дан
Денис - Де
Даша - Даш
*/
	return call_test(__PRETTY_FUNCTION__, []() {
		std::wostringstream sout, sout2;
		RadixTrie rtree;
		rtree.append(L"Данил");
		rtree.append(L"Денис");
		rtree.append(L"Даша");

		rtree.printTree(sout);
		rtree.print(sout2);
		auto res1 = sout.str();
		auto res2 = sout2.str();
		wstring waitRes1 = L"д\n\tа\n\t\tнил$\n\t\tша$\n\tенис$\n";
		wstring waitRes2 = L"данил дан\nдаша даш\nденис де\n";
		return res1 == waitRes1 && res2 == waitRes2;
		});
}

// TODO To config
const wstring testsResDir = L"../tests/results/";
const wstring testsResExt = L".txt";
const wstring testsResSimpleTreeFile = L"../tests/simple-tree.txt";
const wstring testsResNamesShortFile = L"../tests/names-short.txt";

bool all_steps_test_radix_trie() {
	return call_test(__PRETTY_FUNCTION__, []() {
		//// Init data
		//std::vector<wstring> names;
		std::map<wstring, wstring> stepMapArgRes;
		auto names = readTestNames();
		auto pathPrefix = testsResDir;
		auto ext = testsResExt;
		for (auto name : names) {
			auto curFile = pathPrefix + name + ext;
			stepMapArgRes[name] = w_read_file_utf8_be_bom(curFile);
		}
		//// end Init data

		//// Init tree
		std::wostringstream sout; // , sout2;
		RadixTrie rtree;
		rtree.isOutQuotes = true;
		rtree.isOutSpecForDeps = true;
		rtree.sGap = L"  ";
		rtree.isWriteSpecToBeginGap = true;
		//// end Init tree

		auto finalRes = true;
		for (auto name : names) {
			std::wostringstream sout;
			rtree.append(name);
			rtree.printTree(sout);
			const auto res = sout.str();
			const auto waitRes = stepMapArgRes[name];
			if (res != waitRes) {
				finalRes = false;
				outFactAndWaitResults(name, res, waitRes);
				break;
			}
		}
		return finalRes;
		});
}

bool _share_test_print_methods(RadixTrie& rtree, const wstring& resFile, std::function<void(RadixTrie&, std::wostringstream&)> fnTreeOps) {
	addTestNames(rtree);
	std::wostringstream sout;
	fnTreeOps(rtree, sout);
	auto res = sout.str();
	wstring waitRes = w_read_file_utf8_be_bom(resFile);
	bool finalRes = true;
	if (res != waitRes) {
		finalRes = false;
		outFactAndWaitResults(L"simple_tree_test_radix_trie", res, waitRes);
	}
	return finalRes;
}

bool simple_tree_test_radix_trie() {
	return call_test(__PRETTY_FUNCTION__, []() {
		RadixTrie rtree;
		rtree.isOutSpecForDeps = false;
		rtree.sGap = L"  ";
		return _share_test_print_methods(rtree, testsResSimpleTreeFile, [](RadixTrie& rtree, std::wostringstream& sout) {
			rtree.printTree(sout);
		});
	});
}

bool name_short_lines_test_radix_trie() {
	return call_test(__PRETTY_FUNCTION__, []() {
		RadixTrie rtree;
		return _share_test_print_methods(rtree, testsResNamesShortFile, [](RadixTrie& rtree, std::wostringstream& sout) {
			rtree.print(sout);
			});
	});
}

void init_base_fixtures() {
//#ifdef AUTO_CHANGE_CODEPAGE_FOR_WIN
//	systemSpecificInOutInit();
//#endif
}

struct Init {
	Init(std::function<void()> init_func) {
		init_func();
	}
};
#define INIT(init_func) struct Init init(init_func);

BOOST_AUTO_TEST_SUITE(allocator_test_suite)
INIT(init_base_fixtures)

BOOST_AUTO_TEST_CASE(test_of_nickname)
{
	BOOST_CHECK(trivial_test_radix_trie());
	BOOST_CHECK(utf8_test_radix_trie());
	BOOST_CHECK(simple_tree_test_radix_trie());
	BOOST_CHECK(name_short_lines_test_radix_trie());
	/*BOOST_CHECK(all_steps_test_radix_trie());*/
}

BOOST_AUTO_TEST_SUITE_END()
