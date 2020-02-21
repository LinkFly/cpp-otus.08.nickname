#pragma once

#include <string>
#include <array>
#include <limits>

using std::string;

const uint8_t size = std::numeric_limits<uint8_t>::max(); // children of nodes count (for support any single byte encoding and utf8)

struct Node {
	using IdxChar = char;
	using children_size_t = std::remove_const<decltype(size)>::type;
	string label;
	bool isEnd{};
	children_size_t childrenCount = 0;
	std::array<std::unique_ptr<Node>, size> children;

	bool isEmpty(const std::unique_ptr<Node>& node) {
		return node.get() == nullptr;
	}

	std::unique_ptr<Node>& getNode(IdxChar ch = 0) {
		checkChar(ch);
		children_size_t& idx = reinterpret_cast<children_size_t&>(ch);
		auto& curNode = children[idx];
		if (!isEmpty(curNode)) {
			return curNode;
		}
		else {
			auto newNode = std::make_unique<Node>();
			return setNode(idx, newNode);
		}
	}

	std::unique_ptr<Node>& setNode(IdxChar ch, std::unique_ptr<Node>& node) {
		children_size_t& idx = reinterpret_cast<children_size_t&>(ch);
		auto& curNode = children[idx];
		if (!isEmpty(node)) {
			if (isEmpty(curNode)) {
				++childrenCount;
			}
		}
		curNode.swap(node);
		return curNode;
	}
	
	void forEach(std::function<void(std::unique_ptr<Node>&, children_size_t idx, bool isLast)> fn) {
		decltype(childrenCount) order = 0;
		for (std::remove_const<children_size_t>::type i = 0; i < static_cast<children_size_t>(children.size()); ++i) {
			std::unique_ptr<Node>& node = children[i];
			if (!isEmpty(node)) {
				++order;
				fn(node, i, order == childrenCount);
			}
		}
	}

	bool isNoChildren() {
		return childrenCount == 0;
	}

private:
	static void checkChar(IdxChar ch) {
		if (ch == 0) {
			error("Bad char\n");
		}
	}
};
