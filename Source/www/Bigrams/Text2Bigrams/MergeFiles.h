#include <vector>
#include <fstream>

class MergeSortedFiles
{
public:
	MergeSortedFiles() {};
	MergeSortedFiles(std::vector<std::string> filenames, std::string outputFile);
	virtual ~MergeSortedFiles();
	void  Merge();

protected:
	std::vector<std::ifstream*> InputFiles;
	std::ofstream OutputFile;
	bool  ReadLine(size_t fileNo, std::string& line);
	void Close();
};
