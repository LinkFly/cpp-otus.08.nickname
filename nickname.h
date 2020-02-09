#pragma once

#include "share.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <cwctype>
#include <sstream>
#include <stdio.h>

#include "node.h"
#include "utf8-utils.h"

using std::cout;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;

using utf8_utils::readStartedPlusWholeSym;

class RadixTrie {
	std::unique_ptr<Node> root;
	const string gapEnd = u8"└";
	const string gapHere = u8"├";
	const string gapNext = u8"│";

	static void append(std::unique_ptr<Node>& node, const string& label) {
		if (node.get() == nullptr) {
			auto newNode = std::make_unique<Node>();
			node.swap(newNode);
			node->label = label;
			node->isEnd = true;
			return;
		}

		bool isNoChildren = node->isNoChildren();

		if (node->label == "") {
			if (isNoChildren) {
				node->label = label;
				node->isEnd = true;
			}
			else {
				append(node->getNode(label[0]), label);
			}
			return;
		}

		auto pair = std::mismatch(label.begin(), label.end(), node->label.begin(), node->label.end());
		// Iterators to not equal symbols
		auto st1 = std::get<0>(pair);
		auto st2 = std::get<1>(pair);
		// Is equal
		if ((st1 == label.end()) && (st2 == node->label.end())) {
			node->isEnd = true;
			return;
		}

		auto pos1 = st1 - label.begin();
		auto pos2 = st2 - node->label.begin();
		// share part
		string share = label.substr(0, pos1);

		// not equal parts
		string restLabel = label.substr(pos1);
		string restNode = node->label.substr(pos2);

		bool isShare = share != "";
		bool isLabelInNode = node->label != "";
		bool isRestInLabel = restLabel != "";
		bool isRestInNode = restNode != "";
		if (!isLabelInNode) {
			if (isNoChildren) {
				node->label = label;
				node->isEnd = true;
			}
			else {
				auto wch = label[0];
				append(node->getNode(wch), label);
			}
		}
		else if (isLabelInNode) {
			if (!isShare) {
				// here will be node after swap
				auto oldNode = std::make_unique<Node>();
				oldNode.swap(node);
				append(node->getNode(label[0]), label);
				node->setNode(oldNode->label[0], oldNode);
			}
			else {
				if (!isRestInLabel || !isRestInNode) {
					if (!isRestInLabel && isRestInNode) {
						auto oldNode = std::make_unique<Node>();
						oldNode.swap(node);
						node->label = label;
						node->isEnd = true;
						oldNode->label = restNode;
						node->setNode(oldNode->label[0], oldNode);
					}
					else if (!isRestInNode && isRestInLabel) {
						append(node->getNode(restLabel[0]), restLabel);
					}
				}
				else if (isRestInLabel && isRestInNode) {
					auto newNode = std::make_unique<Node>();
					newNode->label = share;
					append(newNode->getNode(restLabel[0]), restLabel);
					node->label = restNode;
					node.swap(newNode);
					// in newNode now - node, require fix name
					auto& oldRoot = newNode;
					node->setNode(oldRoot->label[0], oldRoot);

				}
			}
		}
	}

	string _prepareLabel(string& label) {
		string res = "";
		auto sout = std::make_unique<std::ostringstream>();
		for (uint8_t ch : label) {
			(*sout) << "\\x" << std::hex << static_cast<int>(ch);
			res += sout->str();
			sout->str("");
		}
		return res;
	}

	void _printTree(std::ostream& out, std::unique_ptr<Node>& node, bool topLevel = true, bool isLast = false, std::vector<bool> parentLines = std::vector<bool>{}) {
		if (!topLevel) {
			printGap(out, isLast, parentLines);
			parentLines.push_back(!isLast);
		}
		string preparedLabel;
		if (isOutCodesInPrintTree) {
			preparedLabel = _prepareLabel(node->label/*, isErr*/);
		}
		string endMark = node->isEnd ? "$" : "";
		out << (isOutQuotes ? "\"" : "") << (isOutCodesInPrintTree ? preparedLabel : node->label) << (isOutQuotes ? "\"" : "") << endMark << endl;
		node->forEach([this, &out, &parentLines](
			std::unique_ptr<Node>& node, [[maybe_unused]] Node::children_size_t idx, bool isLast) {
			_printTree(out, node, false, isLast, parentLines);
			});
		if (!topLevel) {
			parentLines.pop_back();
		}
	}

	// For _printTree
	void printGap(std::ostream& out, bool isLast, std::vector<bool> parentLines) {
		bool isCorrectGap = isOutSpecForDeps && isWriteSpecToBeginGap;
		for (bool isNext : parentLines) {
			if (isOutSpecForDeps) {
				if (isNext) {
					out << gapNext;
				}
			}
			out << (isCorrectGap && isNext ? sGap.substr(1) : sGap);
		}
		if (isOutSpecForDeps) {
			out << (isLast ? gapEnd : gapHere);
		}
		out << (isCorrectGap ? sGap.substr(1) : sGap);
	}

	void _print(std::ostream& out, std::unique_ptr<Node>& node, string label = "", string path = "") {
		if (path == "") {
			path = node->label;
		}
		string curLabel = label + node->label;
		if (node->isEnd) {
			auto lastPath = getPathPart(label, node);
			out << curLabel << nameSep << lastPath << endl;
		}
		node->forEach([this, &curLabel, &out](std::unique_ptr<Node>& node, [[maybe_unused]] Node::children_size_t idx, [[maybe_unused]] bool isLast) {
			_print(out, node, curLabel, "");
		});
	}

	string getPathPart(string& started, std::unique_ptr<Node>& node) {
		if (!isUseUTF8) // maybe unusable (maybe only to get firstChar for not utf8)
			return started + string{ node->label[0] };
		return readStartedPlusWholeSym(started, node->label);
	}
public:
	// Config (set it after create instance)
	bool isOutQuotes = false;
	bool isOutSpecForDeps = true;
	string sGap = "\t";
	string nameSep = " "; // maybe " - " for consistent with task description
	bool isWriteSpecToBeginGap = true;
	bool isOutCodesInPrintTree = false;
	bool isUseUTF8 = true;

	void append(const string& label) {
		append(root, label);
	}
	void printTree(std::ostream& out = cout) {
		_printTree(out, root);
	}
	void print(std::ostream& out = cout) {
		_print(out, root, "", "");
	}

	/////////// Extra ///////////////
	void append(const wstring& label) {
		append(root, wstring_to_utf8(label));
	}
};
