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

int main(int argc, char** argv) {;
	RadixTrie rtree;
	rtree.isOutQuotes = true;
	rtree.sGap = "  ";

	Processing proc{ rtree };
	if (argc == 2) {
		if (fs::exists(argv[1])) {
			std::ifstream fin(argv[1], std::ios::in);
			proc.processInput(fin);
		}
		else {
			string errMsg = "file " + string{ argv[2] } +" does not exists";
			error(errMsg);
		}
	}
	else {
		proc.processInput(std::cin);
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
