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
	using converter_t = std::codecvt_utf8<wchar_t>;
	std::unique_ptr< converter_t > converter = std::make_unique< converter_t >();
	std::locale utf8_locale = std::locale(std::locale{}, converter.get());
	/*std::locale utf8_locale = std::locale(std::locale{}, new std::codecvt_utf8<wchar_t>{});*/
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
	rtree.isOutQuotes = false;
	rtree.isOutSpecForDeps = false;
	rtree.sGap = L"  ";

	// TODO!!!!! Добавить обработку файла данных и потока ввода 
	std::vector names = {
		L"aleksey",
		L"sasha",
		L"aleks",
		L"alek",
		L"alesha",
		L"maksim"
	};

	for (auto name : names) {
		rtree.append(name);
	}

	rtree.print();
	wcout << endl;
	rtree.printTree();

	return 0;
}
