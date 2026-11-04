#include <vector>
#include <fstream>
#include <zlib.h>
#include "nlp_base/common/utilit.h"

class MergeSortedFiles
{
public:
	MergeSortedFiles() {};
	MergeSortedFiles(std::vector<std::string> filenames, std::string outputFile);
	virtual ~MergeSortedFiles();
	void  Merge();

protected:
	std::vector<std::ifstream*> InputFiles;
    gzFile OutputFile;
	bool  ReadLine(size_t fileNo, std::string& line);
	void Close();
};
