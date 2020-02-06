#pragma once

#include <vector>
#include <fstream>
#include <string>
#include <iostream>

using std::string;
using std::wstring;
using std::cout;

const wstring testsNamesFile = L"../tests/names.txt";

//// General utils
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
void split(const std::wstring & str, Container & cont, wchar_t delim = ' ')
{
	std::wstringstream ss(str);
	std::wstring token;
	while (std::getline(ss, token, delim)) {
		cont.push_back(token);
	}
}
//// end General utils

std::vector<wstring> readTestNames() {
	wstring allNames = w_read_file_utf8_be_bom(testsNamesFile);
	std::vector<wstring> names;
	split(allNames, names, L'\n');
	return names;
}

void outFactAndWaitResults(const wstring& name, const wstring& res, const wstring& waitRes) {
	const string sName(name.begin(), name.end());
	const string sRes(res.begin(), res.end());
	const string sWaitRes(waitRes.begin(), waitRes.end());
	cout << "\nFailed checking: " << sName << "\n"
		<< "Fact Res:\n---------\n" << sRes << "\n--------\n"
		<< "Wait res:\n---------\n" << sWaitRes << "\n--------\n"
		<< endl;
}