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
