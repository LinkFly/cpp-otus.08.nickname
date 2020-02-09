#pragma once

#include <string>
#include <bitset>
#include <array>
//#include <type_traits>

using std::string;

// constexpr uint8_t size = 26 + 33;  // eng + rus
constexpr uint8_t size = 255; // any byte (any codepage)
//constexpr uint16_t size = 65535; // any byte (any codepage)

struct Node {
	using IdxChar = char;
	using children_size_t = decltype(size);
	string label;
	bool isEnd{};
	std::array<std::unique_ptr<Node>, size> children;
	// TODO!! Пересмотреть использовать bitset'a
	std::bitset<size> busySet = 0;

	std::unique_ptr<Node>& getNode(IdxChar ch = 0) {
		checkChar(ch);

		// TODO!!!!! Возможно Упразднить bitset - считать занятость/не занятость по наличию null/not null, в соотв. ячейке
		children_size_t& idx = ch;
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

	void setNode(IdxChar ch, std::unique_ptr<Node>& node) {
		children_size_t& idx = ch;
		if (node.get() != nullptr) {
			busySet.set(idx);
		}
		children[idx].swap(node);
	}

	bool isNoChildren() {
		return busySet.count() == 0;
	}

private:
	static void checkChar(IdxChar ch) {
		// TODO! Correct error handling
		if (ch == 0) {
			error("Bad char\n");
		}
	}
};
