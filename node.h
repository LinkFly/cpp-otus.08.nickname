#pragma once

#include <string>
#include <bitset>
#include <array>

using std::wstring;

constexpr unsigned size = 26 + 33;  // eng + rus

struct Node {
	wstring label;
	bool isEnd{};
	std::array<std::unique_ptr<Node>, size> children;
	// TODO!! ������������ ������������ bitset'a
	std::bitset<size> busySet = 0;

	std::unique_ptr<Node>& getNode(wchar_t ch = 0) {
		//if (ch == 0) {
		//	return std::make_unique<Node>();
		//}
		checkChar(ch);
		// TODO!!! ���������� bitset - ������� ���������/�� ��������� �� ������� null/not null, � �����. ������
		auto idx = getIdx(ch);
		if (busySet[idx]) {
			return children[idx];
		}
		else {
			busySet.set(idx);
			auto newNode = std::make_unique<Node>();
			children[idx].swap(newNode);
			return children[idx];
		}
	}
	void forEach(std::function<void(std::unique_ptr<Node>&, int idx, bool isLast)> fn) {
		auto count = busySet.count();
		decltype(count) order = 0;
		for (int i = 0; i < busySet.size(); ++i) {
			if (busySet[i]) {
				order++;
				fn(children[i], i, order == count);
			}
		}
	}

	void setNode(wchar_t ch, std::unique_ptr<Node>& node) {
		auto idx = getIdx(ch);
		if (node.get() != nullptr) {
			busySet.set(idx);
		}
		children[idx].swap(node);
	}

	bool isNoChildren() {
		return busySet.to_ullong() == (unsigned long)0;
	}

	static const int8_t getIdx(wchar_t wch) {
		int8_t res;
		if (isIn(wch, L'a', L'z')) {
			res = wch - 'a';
		}
		else if (isIn(wch, L'�'/*rus*/, L'�')) {
			res = wch - L'�'/*rus*/ + 26;
		}
		else if (wch == L'�') {
			res = 26 + 33;
		}
		else {
			res = -1;
		}
		return res;
	}
	static wchar_t getChar(int8_t idx) {
		if (idx < 26) {
			return L'a' + idx;
		}
		else if (idx >= 26 && idx < 26 + 32) {
			return L'�'/*rus*/ + (idx - 26);
		}
		else if (idx == 26 + 33) {
			return L'�';
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
			isIn(ch, L'a', L'z')
			//(ch >= L'a' && ch <= L'z') 
			||
			isIn(ch, L'�'/*rus*/, L'�')
			//(ch >= L'�'/*rus*/ && ch <= L'�')
			||
			ch == L'�'
			)) {
			std::cerr << "Bad char\n";
			exit(1);
		}
	}
};
