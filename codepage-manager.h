#pragma once
/* Only for windows (now) */

#include <stdio.h>
#include <ctype.h>
#include <string>

using std::string;

void setCodePage(const char codepage[]) {
	FILE* pfRes;
	auto nullDev = "null"; // for future extensions: for unix must be /dev/null
	auto setCodePageCmd = ("chcp " + string{ codepage } +" > " + nullDev);
	system(setCodePageCmd.c_str());
}

void setCodePage(string& codepage) {
	setCodePage(codepage.c_str());
}

void setUTF8CodePage() {
	setCodePage("65001");
}

string getStartedCodePage() {
	FILE* pfRes = _popen("chcp", "r");
	char psBuffer[128];
	psBuffer[127] = '\0';
	fgets(psBuffer, 127, pfRes);
	string mesCurCodePage{ psBuffer };
	auto posLastSpace = mesCurCodePage.find_last_of(' ');
	string res = "";
	auto curPos = posLastSpace + 1;
	while (isdigit(mesCurCodePage[curPos])) {
		res += mesCurCodePage[curPos];
		++curPos;
	}
	return res;
}

void systemSpecificInOutInit() {
#ifdef WIN32
#ifdef _MSC_VER
	static auto codepageStarted = getStartedCodePage();
	setUTF8CodePage();
	atexit([]() {
		setCodePage(codepageStarted);
		});
#else
#error "Auto changing codepage allow only VS compilers"
#endif
#else 
#error "Auto changing codepage allow on WIN"
#endif
}