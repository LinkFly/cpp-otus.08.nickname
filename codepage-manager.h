#pragma once
/* Only for windows (now) */

#include <stdio.h>
#include <ctype.h>
#include <string>

using std::string;

void setCodePage(const char codepage[]) {
	auto nullDev = "null"; // for future extensions: for unix must be /dev/null
	auto setCodePageCmd = ("chcp " + string{ codepage } +" > " + nullDev);
	system(setCodePageCmd.c_str());
}

void setCodePage(const string& codepage) {
	setCodePage(codepage.c_str());
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

const string utf8_codepage = "65001";

void systemSpecificInOutInit(string codepage = utf8_codepage) {
#ifdef _MSC_VER
	if (codepage == "utf8")
		codepage = utf8_codepage;
	static auto codepageStarted = getStartedCodePage();
	setCodePage(codepage);
	atexit([]() {
		setCodePage(codepageStarted);
		});
#else
#error "Auto changing codepage allow only VS compilers"
#endif
}
