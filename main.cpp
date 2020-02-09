#include "share.h"

#include <iostream>

#include <string>
#include <locale>
#include <codecvt>
#include <fstream>
#include <vector>
#include <filesystem>
//#include <boost/filesystem.hpp>

#include "nickname.h"

namespace fs = std::filesystem;
//namespace fs = boost::filesystem;

using std::cout;
using std::endl;
using std::string;

template<class InStream, size_t bufsize = 20>
bool readline(InStream& in, string& line) {
	std::getline(in, line);
	return in.eof();
}

class Processing {
	RadixTrie& rtree;
public:
	Processing(RadixTrie& rtree) : rtree{ rtree } {}

	template<typename InStream>
	void processInput(InStream& in) {
		string line;
		bool isEof;
		do {
			isEof = readline(in, line);
			if (line != "") rtree.append(line);
		} while (!isEof);
	}
};

int main(int argc, char** argv) {
	//////// Settings locale and maybe codepage for console (for windows) //////////
	// (for testing with other locale, codepage. For example: test with ./tests/logins-cp1251.txt)
	bool isSingleByteCoding = false;

	if (argc == 4) {
		const string locale = argv[2];
		const string codepage = argv[3];
		if (codepage != "utf8" || codepage != "65001") {
			isSingleByteCoding = true;
		}
		init::run(locale, codepage);
	}
	else if (argc == 1 || argc == 2) {
		init::run();
	}
	else {
		error("Failed: bad count arguments - " + std::to_string(argc - 1) + ", allow: 1, 3 (when 3: file, locale, codepage)");
	}
	//////// end Settings locale and maybe codepage for console (for windows) //////////

	// For debugging
	/*system("chcp");*/

	RadixTrie rtree;
	if (isSingleByteCoding) {
		rtree.isUseUTF8 = false;
		rtree.isOutSpecForDeps = false;
	}
	rtree.isOutQuotes = true;
	rtree.sGap = "  ";
	std::unique_ptr<std::istream> pCurInput;
	
	if (argc >= 2) {
		if (fs::exists(argv[1])) {
			pCurInput = std::make_unique<std::ifstream>(argv[1], std::ios::in);
		}
		else {
			string errMsg = "file " + string{ argv[2] } +" does not exists";
			error(errMsg);
		}
	}
	else {
		pCurInput.reset(&std::cin);
	}

	Processing proc{ rtree };
	proc.processInput(*(pCurInput.get()));

	if (pCurInput.get() == &std::cin) {
		pCurInput.reset();
	}
	/* Uncommented for checking to work with utf8 */
	//std::vector names = {
	//	u8"Мxz",
	//	u8"Маша",
	//	u8"Мама",
	//	u8"АЛЕША",
	//	u8"Алексей",
	//	u8"Ыва"
	//};
	//for (auto name : names) {
	//	rtree.append(name);
	//}
	//rtree.isOutCodesInPrintTree = true;
	//////////////////////////////

	rtree.print();
	rtree.printTree();

	return 0;
}
