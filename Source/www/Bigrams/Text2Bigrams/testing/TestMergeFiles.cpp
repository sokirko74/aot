#include "www/Bigrams/Text2Bigrams/MergeFiles.h"
#include <streambuf>

std::string read_file(std::string fileName) {
	std::ifstream t(fileName);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	return str;
}

int main(int argc, char* argv[]) {
	std::vector<std::string> input;
	for (int i = 1; i < argc - 1; i++) {
		input.push_back(argv[i]);
	}
	std::string outfile = argv[argc - 1];
	MergeSortedFiles m(input, outfile);
	m.Merge();
	if (read_file(outfile) != read_file("canon.txt")) {
		exit(1);
	}
	exit(0);
	 
} 