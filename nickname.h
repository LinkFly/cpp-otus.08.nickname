#pragma once

#include "share.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <cwctype>
//#include <stack>

#include "node.h"

using std::cout;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;

// TODO! Share with tolower (maybe sharing code up/low char)
static wchar_t toWCharUp(wchar_t ch) {
	if (ch >= L'a' && ch <= L'z') {
		return L'A' + (ch - L'a');
	}
	else if (ch >= L'а'/*rus*/ && ch <= L'я') {
		return L'А'/*rus*/ + (ch - L'а'/*rus*/);
	}
	else if (ch == L'ё') {
		return L'Ё';
	}
	else return ch;
}

wstring upFirstChar(wstring str) {
	wchar_t upCh = toWCharUp(str[0]);
	return wstring{upCh} + str.substr(1);
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
			auto newNode = std::make_unique<Node>();
			node.swap(newNode);
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

	void _printTree(std::wostream& out, std::unique_ptr<Node>& node, bool topLevel = true, bool isLast = false, std::vector<bool> parentLines = std::vector<bool>{}) {
		if (!topLevel) {
			printGap(out, isLast, parentLines);
			parentLines.push_back(!isLast);
		}
		wstring endMark = node->isEnd ? L"$" : L"";
		out << (isOutQuotes ? L"\"" : L"") << node->label << (isOutQuotes ? L"\"" : L"") << endMark << L'\n';
		node->forEach([this, &out, &parentLines](
			std::unique_ptr<Node>& node, [[maybe_unused]] int idx, bool isLast) {
			_printTree(out, node, false, isLast, parentLines);
			});
		if (!topLevel) {
			parentLines.pop_back();
		}
	}
	// For _printTree
	void printGap(std::wostream& out, bool isLast, std::vector<bool> parentLines) {
		bool isCorrectGap = isOutSpecForDeps && isWriteSpecToBeginGap;
		for (bool isNext : parentLines) {
			if (isOutSpecForDeps) {
				if (isNext) {
					out << GAP_NEXT;
				}
			}
			out << (isCorrectGap && isNext ? sGap.substr(1) : sGap);
		}
		if (isOutSpecForDeps) {
			out << (isLast ? GAP_END : GAP_HERE);
		}
		out << (isCorrectGap ? sGap.substr(1) : sGap);
	}

	void _print(std::wostream& out, std::unique_ptr<Node>& node, wstring label = L"", wstring path = L"") {
		if (path == L"") {
			path = node->label;
		}
		wstring curLabel = label + node->label;
		
		if (node->isEnd) {
			if (!isUpFstCharOnPrint) {
				out << curLabel << " " << path << endl;
			}
			else {
				out << upFirstChar(curLabel) << " " << upFirstChar(path) << endl;
			}
		}
		node->forEach([this, &curLabel, &out](std::unique_ptr<Node>& node, int idx, [[maybe_unused]] bool isLast) {
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
