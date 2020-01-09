#pragma once

#include "share.h"

#include <array>
#include <iostream>
#include <bitset>
#include <algorithm>
#include <functional>

using std::cout;
using std::endl;
using std::string;

template<unsigned size = 26>
class RadixTrie {
	struct Node {
		string label;
		bool is_end{};
		std::array<std::unique_ptr<Node>, size> children;
		std::bitset<size> busySet = 0;
		std::unique_ptr<Node>& getNode(char ch) {
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
		static auto getChar(int idx) {
			return 'a' + idx;
		}
	private:
		static void checkChar(char ch) {
			// TODO! Correct error handling
			if (ch < 'a' || ch > 'z') {
				std::cerr << "Bad char\n";
				exit(1);
			}
		}
		static auto getIdx(char ch) {
			return ch - 'a';
		}
	};
	std::unique_ptr<Node> root;
public:
	static void append(std::unique_ptr<Node>& node, const string& label) {
		if (node.get() == nullptr) {
			node.reset(new Node());
			node->label = label;
			return;
		}
		if (node->label == "") {
			node->label = label;
			return;
		}
		auto pair = std::mismatch(label.begin(), label.end(), node->label.begin());
		auto pos1 = std::get<0>(pair) - label.begin();
		auto pos2 = std::get<1>(pair) - node->label.begin();
		string share = label.substr(0, pos1);
		string rest1 = label.substr(pos1);
		string rest2 = node->label.substr(pos2);

		node->label = share;
		append(node->getNode(rest1[0]), rest1);
		append(node->getNode(rest2[0]), rest2);
	}
	static void printTree(std::ostream& out, std::unique_ptr<Node>& node, int level = 0) {
		printGap(out, level);
		out << node->label << endl;
		node->forEach([&level, &out](std::unique_ptr<Node>& node, [[maybe_unused]] int idx) {
			printTree(out, node, level + 1);
			});
	}
	static void print(std::ostream& out, std::unique_ptr<Node>& node, string label = "", string path = "") {
		if (path == "") {
			path = node->label;
		}
		string curLabel = label + node->label;
		if (node->isNoChildren()) {
			out << curLabel << " " << path << endl;
		}
		node->forEach([&path, &curLabel, &out](std::unique_ptr<Node>& node, int idx) {
			char ch = Node::getChar(idx);
			string curPath = path + string{ ch };
			print(out, node, curLabel, curPath);
			});
	}

	void append(const string& label) {
		append(root, label);
	}
	void printTree(std::ostream& out = cout) {
		printTree(out, root);
	}
	void print(std::ostream& out = cout) {
		print(out, root);
	}
private:
	static void add(std::unique_ptr<Node>& node, const string& label) {
		char fstChar = label[0];

	}
	static void printGap(std::ostream& out, int n) {
		while (n--) {
			out << '\t';
		}
	}
};
