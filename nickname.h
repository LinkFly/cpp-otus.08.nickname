﻿#pragma once

#include "share.h"

#include <iostream>
#include <algorithm>
#include <functional>
#include <cwctype>
#include <sstream>
#include <stdio.h>

// TODO!!!!! Share code with tests-utils.h
//#include <codecvt>
//std::string wstring_to_utf8(const std::wstring& str)
//{
//	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
//	return myconv.to_bytes(str);
//}

#include "node.h"

using std::cout;
using std::wcout;
using std::endl;
using std::string;
using std::wstring;

//mstring upFirstChar(mstring str) {
//	char upCh = std::towupper(str[0]);
//	return mstring{upCh} + str.substr(1);
//}
//
//mstring tolower(mstring label) {
//	std::transform(label.begin(), label.end(), label.begin(), std::towlower);
//	return label;
//}

const wchar_t GAP_END = L'└';
const wchar_t GAP_HERE = L'├';
const wchar_t GAP_NEXT = L'│';

class RadixTrie {
	std::unique_ptr<Node> root;
	const string gapEnd = RadixTrie::prepGap(GAP_END);
	const string gapHere = prepGap(GAP_HERE);
	const string gapNext = prepGap(GAP_NEXT);

	static string prepGap(wchar_t gap) {
		return wstring_to_utf8(wstring{ gap });
	}

	static void append(std::unique_ptr<Node>& node, const wstring& initLabel) {
		append(node, wstring_to_utf8(initLabel));
	}
	static void append(std::unique_ptr<Node>& node, const string& initLabel) {
		// prepare label
		//wstring label = tolower(initLabel);
		const string& label = initLabel;
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

	string _prepareLabel(string& label, bool &isErr) {
		isErr = false;
		string res = "";
		if (isOutCodesInPrintTree) {
			auto sout = std::make_unique<std::ostringstream>();
			for (uint8_t ch : label) {
				(*sout) << "\\x" << std::hex << static_cast<int>(ch);
				res += sout->str();
				sout->str("");
			}
			return res;
		}

		//bool bSoutIsInit = false;
		//std::unique_ptr<std::ostringstream> sout;
		//auto maybe_init_sout = [&bSoutIsInit , &sout]() {
		//	if (!bSoutIsInit) {
		//		sout = std::make_unique<std::ostringstream>();
		//		bSoutIsInit = true;
		//	}
		//};
		for (uint8_t ch : label) {
			if (ch <= 127) {
				res += ch;
			}
			else {
				isErr = true;
				return "";
			}
		}
		return res;
	}

	void _printTree(std::ostream& out, std::unique_ptr<Node>& node, bool topLevel = true, bool isLast = false, std::vector<bool> parentLines = std::vector<bool>{}) {
		if (!topLevel) {
			printGap(out, isLast, parentLines);
			parentLines.push_back(!isLast);
		}
		bool isErr;
		string preparedLabel = _prepareLabel(node->label, isErr);
		if (isErr) {
			std::cerr << "------- ERROR _printTree: Not implemented for symbols with code > 127 ------- \n";
			exit(-1);
		}
		string endMark = node->isEnd ? "$" : "";
		//preparedLabel = "X";
		out << (isOutQuotes ? "\"" : "") << preparedLabel << (isOutQuotes ? "\"" : "") << endMark << endl;
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
			out << curLabel << " " << path << endl;
			/*if (!isUpFstCharOnPrint) {
				out << curLabel << " " << path << endl;
			}
			else {
				out << upFirstChar(curLabel) << " " << upFirstChar(path) << endl;
			}*/
		}
		node->forEach([this, &curLabel, &out](std::unique_ptr<Node>& node, Node::children_size_t idx, [[maybe_unused]] bool isLast) {
			char ch = Node::getChar(idx);
			string curPath = curLabel + string{ ch };
			_print(out, node, curLabel, curPath);
			});
	}

public:
	// Config (set it after create instance)
	bool isOutQuotes = false;
	bool isOutSpecForDeps = false;
	/*bool isUpFstCharOnPrint = false;*/
	string sGap = "\t";
	bool isWriteSpecToBeginGap = true;
	bool isOutCodesInPrintTree = false;

	void append(const wstring& label) {
		append(root, label);
	}
	void printTree(std::ostream& out = cout) {
		_printTree(out, root);
	}
	void print(std::ostream& out = cout) {
		_print(out, root, "", "");
	}
};
