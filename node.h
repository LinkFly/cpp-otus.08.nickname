#pragma once

#include <string>
#include <bitset>
#include <array>
//#include <type_traits>

using std::wstring;

// constexpr uint8_t size = 26 + 33;  // eng + rus
constexpr uint16_t size = (256 * 256) - 1; // any byte (any codepage)

struct Node {
	using children_size_t = decltype(size);
	wstring label;
	bool isEnd{};
	std::array<std::unique_ptr<Node>, size> children;
	// TODO!! Пересмотреть использовать bitset'a
	std::bitset<size> busySet = 0;

	std::unique_ptr<Node>& getNode(wchar_t ch = 0) {
		//if (ch == 0) {
		//	return std::make_unique<Node>();
		//}

		//checkChar(ch);

		// TODO!!!!! Возможно Упразднить bitset - считать занятость/не занятость по наличию null/not null, в соотв. ячейке
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
	void forEach(std::function<void(std::unique_ptr<Node>&, children_size_t idx, bool isLast)> fn) {
		auto count = busySet.count();
		decltype(count) order = 0;
		for (std::remove_const<children_size_t>::type i = 0; i < static_cast<children_size_t>(busySet.size()); ++i) {
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
		//return busySet.to_ullong() == (unsigned long)0;
		return busySet.count() == 0;
	}

	static children_size_t getIdx(wchar_t wch) {
		return wch;
		//int8_t res;
		//if (isIn(wch, L'a', L'z')) {
		//	res = wch - 'a';
		//}
		//else if (isIn(wch, L'а'/*rus*/, L'я')) {
		//	res = wch - L'а'/*rus*/ + 26;
		//}
		//else if (wch == L'ё') {
		//	res = 26 + 33;
		//}
		//else {
		//	res = -1;
		//}
		//return res;
	}
	static wchar_t getChar(children_size_t idx) {
		return idx;
		//if (idx < 26) {
		//	return L'a' + idx;
		//}
		//else if (idx >= 26 && idx < 26 + 32) {
		//	return L'а'/*rus*/ + (idx - 26);
		//}
		//else if (idx == 26 + 33) {
		//	return L'ё';
		//}
		//else return L'\0';
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
			isIn(ch, L'а'/*rus*/, L'я')
			//(ch >= L'а'/*rus*/ && ch <= L'я')
			||
			ch == L'ё'
			)) {
			std::cerr << "Bad char\n";
			exit(1);
		}
	}
};
