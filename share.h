#pragma once

#ifdef _MSC_VER
	#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

//// Configs
#define AUTO_CHANGE_CODEPAGE_FOR_WIN 1
//// end Configs

#ifdef AUTO_CHANGE_CODEPAGE_FOR_WIN
#include "codepage-manager.h"
#endif

