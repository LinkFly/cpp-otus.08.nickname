#pragma once

#include "share.h"


#include <iostream>
#include <algorithm>
#include <functional>
#include <cwctype>

#include "node.h"

using std::cout;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;

wstring upFirstChar(wstring str) {
	return wstring{ std::towupper(str[0]) } +str.substr(1);
}

const wchar_t GAP_END = L'└';
const wchar_t GAP_HERE = L'├';
const wchar_t GAP_NEXT = L'│';

class RadixTrie {
	std::unique_ptr<Node> root;
	
	// TODO! Поискать стандартную ф-ию перевода широких строк к нижнему регистру (также поискать в boost'e)
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

	static void append(std::unique_ptr<Node>& node, const wstring& initLabel) {
		// prepare label
		wstring label = tolower(initLabel);

		if (node.get() == nullptr) {
			node.reset(new Node());
			node->label = label;
			node->isEnd = true;
			return;
		}

		bool isNoChildren = node->isNoChildren();

		if (node->label == L"") {
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
		wstring share = label.substr(0, pos1);

		// not equal parts
		wstring restLabel = label.substr(pos1);
		wstring restNode = node->label.substr(pos2);

		bool isShare = share != L"";
		bool isLabelInNode = node->label != L"";
		bool isRestInLabel = restLabel != L"";
		bool isRestInNode = restNode != L"";
		bool isEndInNode = node->isEnd;

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

	void _printTree(std::wostream& out, std::unique_ptr<Node>& node, int level = 0, bool isLast = false) {
		//const bool isNoChildren = node->isNoChildren();
		printGap(out, level, isLast);
		wstring endMark = node->isEnd ? L"$" : L"";
		out << (isOutQuotes ? L"\"" : L"") << node->label << (isOutQuotes ? L"\"" : L"") << endMark << L'\n';
		node->forEach([this, &level, &out](std::unique_ptr<Node>& node, [[maybe_unused]] int idx, bool isLast) {
			_printTree(out, node, level + 1, isLast);
			});
	}
	// For _printTree
	void printGap(std::wostream& out, int n, bool isLast) {
		while (n--) {
			if (isOutSpecForDeps) {
				auto curGap = isLast ? GAP_END : GAP_HERE;
				out << curGap;
			}
			out << (isWriteSpecToBeginGap ? sGap.substr(1) : sGap);
		}
	}

	void _print(std::wostream& out, std::unique_ptr<Node>& node, wstring label = L"", wstring path = L"") {
		if (path == L"") {
			path = node->label;
		}
		wstring curLabel = label + node->label;
		if (node->isNoChildren()) {
			if (!isUpFstCharOnPrint) {
				out << curLabel << " " << path << endl;
			}
			else {
				out << upFirstChar(curLabel) << " " << upFirstChar(path) << endl;
			}
		}
		node->forEach([this, &path, &curLabel, &out](std::unique_ptr<Node>& node, int idx, bool isLast) {
			wchar_t ch = Node::getChar(idx);
			wstring curPath = curLabel + wstring{ ch };
			_print(out, node, curLabel, curPath);
			});
	}

public:
	// Config (set it after create instance)
	bool isOutQuotes = false;
	bool isOutSpecForDeps = false;
	bool isUpFstCharOnPrint = false;
	wstring sGap = L"\t";
	bool isWriteSpecToBeginGap = true;

	void append(const wstring& label) {
		append(root, label);
	}
	void printTree(std::wostream& out = wcout) {
		_printTree(out, root);
	}
	void print(std::wostream& out = wcout) {
		_print(out, root, L"", L"");
	}
	
};
