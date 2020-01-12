#pragma once

#include "share.h"

#include <array>
#include <iostream>
#include <bitset>
#include <algorithm>
#include <functional>
#include <cwctype>

using std::cout;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;

wstring upFirstChar(wstring str) {
	return wstring{ std::towupper(str[0]) } +str.substr(1);
}

template<unsigned size = 26 + 33> // eng + rus
class RadixTrie {


	struct Node {
		wstring label;
		bool is_end{};
		std::array<std::unique_ptr<Node>, size> children;
		std::bitset<size> busySet = 0;
		std::unique_ptr<Node>& getNode(wchar_t ch = 0) {
			if (ch == 0) {
				return std::make_unique<Node>();
			}
			checkChar(ch);
			auto idx = getIdx(ch);
			if (busySet[idx]) {
				return children[idx];
			}
			else {
				busySet.set(idx);
				return children[idx] = std::make_unique<Node>();
			}
		}
		void forEach(std::function<void(std::unique_ptr<Node>&, int idx)> fn) {
			for (int i = 0; i < busySet.size(); ++i) {
				if (busySet[i]) {
					fn(children[i], i);
				}
			}
		}
		bool isNoChildren() {
			return busySet.to_ullong() == (unsigned long)0;
		}

		static int8_t getIdx(wchar_t wch) {
			int8_t res;
			if (isIn(wch, L'a', L'z')) {
				res = wch - 'a';
			}
			else if (isIn(wch, L'а'/*rus*/, L'я')) {
				res = wch - L'а'/*rus*/ + 26;
			}
			else if (wch == L'ё') {
				res = 26 + 33;
			}
			else {
				res = -1;
			}
			if (res == -11) {
				cout << "test" << endl;
			}
			return res;
		}
		static wchar_t getChar(int8_t idx) {
			if (idx < 26) {
				return L'a' + idx;
			}
			else if (idx >= 26 && idx < 26 + 32) {
				return L'а'/*rus*/ + idx;
			}
			else if (idx == 26 + 33) {
				return L'ё';
			}
			else return L'\0';
		}

	private:
		static bool isIn(wchar_t ch, wchar_t low, wchar_t high) {
			return ch >= low && ch <= high;
		}

		static void checkChar(wchar_t ch) {
			// TODO! Correct error handling
			if (!(
				(ch >= L'a' && ch <= L'z') 
				||
				(ch >= L'а'/*rus*/ && ch <= L'я')
				||
				ch == L'ё'
				)) {
				std::cerr << "Bad char\n";
				exit(1);
			}
		}
	};

	std::unique_ptr<Node> root;

	static wstring tolower(wstring label) {
		std::transform(label.begin(), label.end(), label.begin(), [](wchar_t ch) -> wchar_t {
			if (ch >= L'A' && ch <= L'Z') {
				return L'a' + (ch - L'A');
			}
			else if (ch >= L'А'/*rus*/ && ch <= L'Я') {
				return L'а'/*rus*/ + (ch - L'А'/*rus*/);
			}
			else if (ch == L'Ё') {
				return L'ё';
			}
			else {
				return ch;
			}
			});
		return label;
	}
public:
	static void append(std::unique_ptr<Node>& node, const wstring& initLabel) {
		wstring label = tolower(initLabel);
		if (node.get() == nullptr) {
			node.reset(new Node());
			node->label = label;
			return;
		}
		if (node->label == L"") {
			node->label = label;
			return;
		}
		
		auto pair = std::mismatch(label.begin(), label.end(), node->label.begin(), node->label.end());
		auto st1 = std::get<0>(pair);
		auto st2 = std::get<1>(pair);
		if ((st1 == label.end()) && (st2 == node->label.end())) {
			return;
		}
		auto pos1 = st1 - label.begin();
		auto pos2 = st2 - node->label.begin();
		wstring share = label.substr(0, pos1);
		wstring rest1 = label.substr(pos1);
		wstring rest2 = node->label.substr(pos2);

		if (share == L"") {
			if (node->label != L"") {
				// TODO!!!!!!!!!!1
				/*auto oldNode = node;
				node.reset(node->getNode());
				append(newNode, )*/
			}
			
		}
		else {
			node->label = share;
			if (rest1.size() != 0) {
				append(node->getNode(rest1[0]), rest1);
			}
			if (rest2.size() != 0) {
				append(node->getNode(rest2[0]), rest2);
			}
		}
	}
	static void printTree(std::wostream& out, std::unique_ptr<Node>& node, int level = 0) {
		/*wcout << L"in-printtree\n";*/
		printGap(out, level);
		/*wcout << L"after-gap\n";*/
		out << node->label << L'\n';
		/*wcout << node->label << L'\n';*/
		/*wcout << L"...attention: " << (int)node->label[0] << L"asdf " << L'\n';*/
		/*wcout << L"after-label\n";*/
		node->forEach([&level, &out](std::unique_ptr<Node>& node, [[maybe_unused]] int idx) {
			printTree(out, node, level + 1);
			});
	}
	
	static void print(std::wostream& out, std::unique_ptr<Node>& node, wstring label = "", wstring path = "", bool upFstChar = false) {
		if (path == L"") {
			path = node->label;
		}
		wstring curLabel = label + node->label;
		if (node->isNoChildren()) {
			if (!upFstChar) {
				out << curLabel << " " << path << endl;
			}
			else {
				out << upFirstChar(curLabel) << " " << upFirstChar(path) << endl;
			}
		}
		node->forEach([&path, &curLabel, &out](std::unique_ptr<Node>& node, int idx) {
			wchar_t ch = Node::getChar(idx);
			wstring curPath = path + wstring{ ch };
			print(out, node, curLabel, curPath);
			});
	}

	void append(const wstring& label) {
		append(root, label);
	}
	void printTree(std::wostream& out = wcout) {
		printTree(out, root);
	}
	void print(std::wostream& out = wcout, bool upFstChar = false) {
		print(out, root, L"", L"", upFstChar);
	}
	//static const uint8_t getMagicUTF8CyryllicStartByte() {
	//	wstring cyrStr{ "а" };
	//	cout << cyrStr.size() << endl;
	//	/*wstring cyrStrE{ "Д" };
	//	wstring tmp = cyrStrE.substr(0, 2);
	//	wstring tmp2{ "Д" };
	//	cout << "substr: " << tmp << " " << std::bitset<8>(tmp[1]) << " " << int(tmp == tmp2) << endl;
	//	cout << "\n==== " << std::bitset<8>(cyrStr[0]) << " " << std::bitset<8>(cyrStr[1]) << " \n";
	//	wstring cyrStr2{ "ё" };
	//	cout << "\n==== " << std::bitset<8>(cyrStr2[0]) << " " << std::bitset<8>(cyrStr2[1]) << " \n";
	//	wstring cyrStr3{ "я" };
	//	cout << "\n==== " << std::bitset<8>(cyrStr3[0]) << " " << std::bitset<8>(cyrStr3[1]) << " \n";
	//	std::wwstring cyrStr4{ L"д" };
	//	cout << "\n==== " << std::bitset<16>(cyrStr4[0]) << " \n";
	//	*/
	//	return (uint8_t)10;
	//}

private:
	static void add(std::unique_ptr<Node>& node, const wstring& label) {
		char fstChar = label[0];

	}
	static void printGap(std::wostream& out, int n) {
		while (n--) {
			out << L'\t';
		}
	}
	
};
