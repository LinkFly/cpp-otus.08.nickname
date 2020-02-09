#pragma once

#include <string>
#include <iostream>

namespace utf8_utils {
	using std::cerr;

	using std::string;

	// For saved only this bits
	constexpr uint8_t b11111 = 0xf8;
	constexpr uint8_t b1111 = 0xf0;
	constexpr uint8_t b111 = 0xe0;
	constexpr uint8_t b11 = 0xc0;
	constexpr uint8_t b1 = 0x80;

	// For checkint utf special codes
	constexpr uint8_t utf8_4b_11110 = 0xf0;
	constexpr uint8_t utf8_3b_1110 = 0xe0;
	constexpr uint8_t utf8_2b_110 = 0xc0;
	constexpr uint8_t utf8_1b_0 = 0x00;
	constexpr uint8_t utf8_not_first_10 = 0x80;

	bool check_utf8_not_first(uint8_t byte) {
		return (byte & b11) == utf8_not_first_10;
	}

	uint8_t getRepresentBytesCount(uint8_t byte) {
		if (check_utf8_not_first(byte)) {
			cerr << "======== ERROR: into getRepresentBytesCount working with not started utf8 bytes!!! =======\n";
			exit(-1);
		}
		if ((byte & b11111) == utf8_4b_11110)
			return 4;
		else if ((byte & b1111) == utf8_3b_1110) {
			return 3;
		}
		else if ((byte & b111) == utf8_2b_110) {
			return 2;
		}
		else if ((byte & b1) == utf8_1b_0) {
			return 1;
		}
		else {
			cerr << "======== ERROR: into getRepresentBytesCount not finded true case!!! =======\n";
			exit(-1);
		}
	}

	//// Helpers
	string getUTF8StartedBytes(string& started) {
		auto size = started.size();
		string res = "";
		for (size_t i = 0; i < size; ++i) {
			uint8_t byte = started[i];
			auto count = getRepresentBytesCount(byte);
			auto pred_i = i;
			i += count - 1;
			if (i + 1 == size) {
				res = "";
				break;
			}
			else if (i + 1 > size) {
				res = started.substr(pred_i);
				break;
			}
		}
		return res;
	}
	//// end Helpers

	string readStartedPlusWholeSym(string& started, string& label) {
		if (label == "") {
			return started;
		}
		auto fst = label[0];
		auto curWholeLabel = label;
		auto curStarted = started;
		if (check_utf8_not_first(fst)) {
			auto begin_utf8_bytes = getUTF8StartedBytes(started);
			curWholeLabel = begin_utf8_bytes + curWholeLabel;
			curStarted = curStarted.substr(0, curStarted.size() - begin_utf8_bytes.size());
		}
		auto trueFst = curWholeLabel[0];
		auto count = getRepresentBytesCount(trueFst);
		auto res = curStarted + curWholeLabel.substr(0, count);
		return res;
	}
} // end namespace