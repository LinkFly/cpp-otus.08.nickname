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

bool trivial_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		std::ostringstream sout, sout2;
		RadixTrie rtree;
		rtree.isOutSpecForDeps = false;
		rtree.isOutSpecForDeps = false;
		rtree.append(L"mark");
		rtree.append(L"mast");
		rtree.printTree(sout);
		rtree.print(sout2);
		auto res1 = sout.str();
		auto res2 = sout2.str();
		string waitRes1 = "ma\n\trk$\n\tst$\n";
		string waitRes2 = "mark mar\nmast mas\n";
		bool checkRes1 = res1 == waitRes1;
		bool checkRes2 = res2 == waitRes2;
		if (!checkRes1) {
			outFactAndWaitResults("trivial_test", res1, waitRes1);
		}
		if (!checkRes2) {
			outFactAndWaitResults("trivial_test", res2, waitRes2);
		}
		return checkRes1 && checkRes2;
	});
}

bool utf8_test() {
/*
Данил - Дан
Денис - Де
Даша - Даш
*/
	return call_test(__PRETTY_FUNCTION__, []() {
		std::ostringstream sout, sout2;
		RadixTrie rtree;
		rtree.isOutCodesInPrintTree = true;

		rtree.append(L"Данил");
		rtree.append(L"Денис");
		rtree.append(L"Даша");

		/*rtree.printTree(sout);*/
		rtree.print(sout2);
		/*auto res1 = sout.str();*/
		auto res2 = sout2.str();
		/*string waitRes1 = u8"Д\n\tа\n\t\tнил$\n\t\tша$\n\tенис$\n";*/
		string waitRes2 = u8"Данил Дан\nДаша Даш\nДенис Де\n";

		// for simplify debugging
		/*bool check1 = res1 == waitRes1;
		if (!check1) {
			outFactAndWaitResults("utf8_test", res1, waitRes1);
		}*/
		bool check2 = res2 == waitRes2;
		if (!check2) {
			outFactAndWaitResults("utf8_test", res2, waitRes2);
		}
		/*return check1 && check2;*/
		return check2;
		});
}

// TODO To config
const wstring testsResDir = L"../tests/results/";
const wstring testsResExt = L".txt";
const wstring testsResSimpleTreeFile = L"../tests/simple-tree.txt";
const wstring testsResNamesShortFile = L"../tests/names-short.txt";

bool all_steps_test() {
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
		rtree.sGap = "  ";
		rtree.isWriteSpecToBeginGap = true;
		//// end Init tree

		auto finalRes = true;
		for (auto name : names) {
			std::ostringstream sout;
			rtree.append(name);
			rtree.printTree(sout);
			const auto res = sout.str();
			const auto wwaitRes = stepMapArgRes[name];
			string waitRes = wstring_to_utf8(wwaitRes);
			if (res != waitRes) {
				finalRes = false;
				outFactAndWaitResults(wstring_to_utf8(name), res, waitRes);
				break;
			}
		}
		return finalRes;
		});
}

bool _share_test_print_methods(RadixTrie& rtree, const wstring& resFile, std::function<void(RadixTrie&, std::ostringstream&)> fnTreeOps) {
	addTestNames(rtree);
	std::ostringstream sout;
	fnTreeOps(rtree, sout);
	auto res = sout.str();
	wstring wwaitRes = w_read_file_utf8_be_bom(resFile);
	string waitRes = wstring_to_utf8(wwaitRes);
	bool finalRes = true;
	if (res != waitRes) {
		finalRes = false;
		outFactAndWaitResults("simple_tree_test", res, waitRes);
	}
	return finalRes;
}

bool simple_tree_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		RadixTrie rtree;
		rtree.isOutSpecForDeps = false;
		rtree.sGap = "  ";
		return _share_test_print_methods(rtree, testsResSimpleTreeFile, [](RadixTrie& rtree, std::ostringstream& sout) {
			rtree.printTree(sout);
		});
	});
}

bool name_short_lines_test() {
	return call_test(__PRETTY_FUNCTION__, []() {
		RadixTrie rtree;
		return _share_test_print_methods(rtree, testsResNamesShortFile, [](RadixTrie& rtree, std::ostringstream& sout) {
			rtree.print(sout);
			});
	});
}

void init_base_fixtures() {
	// Init code must be here
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
	BOOST_CHECK(trivial_test());
	BOOST_CHECK(utf8_test());
	BOOST_CHECK(simple_tree_test());
	BOOST_CHECK(name_short_lines_test());
	BOOST_CHECK(all_steps_test());
}
BOOST_AUTO_TEST_SUITE_END()
