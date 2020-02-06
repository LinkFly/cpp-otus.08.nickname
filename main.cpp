#include "share.h"

#include <iostream>

#include <string>
#include <locale>
#include <codecvt>
#include <fstream>

#include "nickname.h"

using std::cout;
using std::endl;
using std::string;

struct InOutInit {
	std::locale utf8_locale = std::locale(std::locale{}, new std::codecvt_utf8<wchar_t>{});
	void init(std::wostream& out = std::wcout) {
		out.imbue(utf8_locale);
	}
};

int main() {
#ifdef AUTO_CHANGE_CODEPAGE_FOR_WIN
	systemSpecificInOutInit();
#endif
	InOutInit inOutInit;
	inOutInit.init();
	
	RadixTrie rtree;
	rtree.isOutQuotes = true;
	rtree.isOutSpecForDeps = true;
	rtree.sGap = L"  ";
	// TODO!!!!! Добавить обработку файла данных и потока ввода 
	rtree.append(L"mark");
	rtree.append(L"mast");
	rtree.append(L"Марк");
	/*rtree.append(L"Марк");
	rtree.append(L"Mark");
	rtree.append(L"Марк");
	rtree.append(L"Маст");*/

	rtree.print();
	rtree.printTree();

	return 0;
}
