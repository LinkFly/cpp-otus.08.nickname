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

using std::wstring;
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

//// Utils
//wstring read_file(wstring filename) {
//	std::wifstream fin(filename, std::ios::in);
//	wstring res;
//	for (wstring line; std::getline(fin, line);)
//		res += (line + L"\n");
//	return res;
//}

wstring w_read_file_utf8_be_bom(wstring filename, wchar_t checkCodepage = 0xfeff/*UTF16-BE*/) {
	std::ifstream fin(filename, std::ios::in);
	char buf[2];
	auto toWChar = [](char* buf2ch) -> wchar_t {
		return ((unsigned short)buf2ch[0] << 8) + (unsigned char)buf2ch[1];
	};
	wstring res = L"";
	if (checkCodepage != L'0') {
		fin.read(buf, 2);
		wchar_t wch = toWChar(buf);
		if (wch != checkCodepage) {
			string sFile = string{ filename.begin(), filename.end() };
			std::cerr << "Bad codepage in file: " << sFile << ". Got: " << std::hex << wch << " Waited: " << std::hex << checkCodepage << endl << endl;
			exit(-1);
		}
	}
	while (true) {
		fin.read(buf, 2);
		if (!fin) {
			break;
		}
		res += toWChar(buf);
	}
	return res;
}

template <class Container>
void split(const std::wstring& str, Container& cont, wchar_t delim = ' ')
{
	std::wstringstream ss(str);
	std::wstring token;
	while (std::getline(ss, token, delim)) {
		cont.push_back(token);
	}
}
//// end Utils

// TODO To config
const wstring testsNamesFile = L"../tests/names.txt";
const wstring testsResDir = L"../tests/results/";
const wstring testsResExt = L".txt";

bool all_steps_test_radix_trie() {
	return call_test(__PRETTY_FUNCTION__, []() {
		//// Init data
		std::vector<wstring> names;
		std::map<wstring, wstring> stepMapArgRes;
		wstring allNames = w_read_file_utf8_be_bom(testsNamesFile);
		split(allNames, names, L'\n');
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
				const string sName(name.begin(), name.end());
				const string sRes(res.begin(), res.end());
				const string sWaitRes(waitRes.begin(), waitRes.end());
				cout << "\nFailed checking: " << sName << "\n"
					<< "Fact Res: " << sRes << "\n"
					<< "Wait res: " << sWaitRes << "\n"
					<< endl;
				break;
			}
		}
		return finalRes;
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
	BOOST_CHECK(all_steps_test_radix_trie());
	
}

BOOST_AUTO_TEST_SUITE_END()
