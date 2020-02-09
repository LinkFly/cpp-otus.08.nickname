#pragma once

#ifdef _MSC_VER
	#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

// For working std::codecvt_utf8<wchar_t> into c++17 without warning
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

//// Configs
#define AUTO_CHANGE_CODEPAGE_FOR_WIN 1
//// end Configs

#if defined(WIN32) && defined(AUTO_CHANGE_CODEPAGE_FOR_WIN)
#include "codepage-manager.h"
#endif

#include <locale>
#include <codecvt>
#include <iostream>
#include <string>

using std::string;

/////////// Init code ////////////
// For to run init code
namespace init {
	struct init_t {
		void run(const string& locale, [[maybe_unused]] const string& codepage) {
			setlocale(LC_ALL, locale.c_str());
#if defined(WIN32) && defined(AUTO_CHANGE_CODEPAGE_FOR_WIN)
			systemSpecificInOutInit(codepage);
#endif

//////// Reserv
			//std::locale utf8_locale = std::locale(std::locale{}, new std::codecvt_utf8<wchar_t>{});
			//std::wcout.imbue(utf8_locale);
		}
	};
	void run(const string& locale = "Russian_Russia.UTF8", string codepage = "utf8") {
		init_t init;
		init.run(locale, codepage);
	}
}

std::string wstring_to_utf8(const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(str);
}

constexpr int defaultErrCode = -1;

void error(const char* errMsg, int errCode = defaultErrCode) {
	std::cerr << errMsg;
	exit(errCode);
}

void error(const string& errMsg, int errCode = defaultErrCode) {
	error(errMsg.c_str(), errCode);
}
