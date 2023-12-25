#include "MergeFiles.h"

#include "morph_dict/common/utilit.h"
#include <assert.h>
#include <queue>
#include <string>


MergeSortedFiles::MergeSortedFiles(std::vector<std::string> filenames, std::string outputFile) {
    for (auto f : filenames)
    {
        auto inp = new std::ifstream(f);
        assert(inp->is_open());
        InputFiles.push_back(inp);
    }
    OutputFile = gzopen(outputFile.c_str(), "wb");
    if (!OutputFile) {
        throw CExpc("cannot write to %s", outputFile.c_str());
    }

}

void MergeSortedFiles::Close() {
    for (auto f : InputFiles) {
        f->close();
        delete f;
    }
    InputFiles.clear();
    if (OutputFile != nullptr) {
        gzclose(OutputFile);
        OutputFile = nullptr;
    }
}


MergeSortedFiles::~MergeSortedFiles() {
    Close();
}

bool  MergeSortedFiles::ReadLine(size_t fileNo, std::string& line)
{
    if (InputFiles[fileNo]->eof()) return false;
    if (std::getline(*InputFiles[fileNo], line)) {
        return true;
    }
    return false;
}

struct TNode {
    size_t FileNo;
    std::string Line;
    bool operator > (const TNode& n) const {
        return Line < n.Line;
    }
    bool operator < (const TNode& n) const {
        return Line > n.Line;
    }
};

void  MergeSortedFiles::Merge()
{
    std::priority_queue<TNode> pq;

    // push the head nodes of all 
    // the k lists in 'pq' 
    std::string line;
    for (size_t fileNo = 0; fileNo < InputFiles.size(); fileNo++) {
        if (ReadLine(fileNo, line)) {
            pq.push({ fileNo, line });
        }
    }
 
    // loop till 'pq' is not empty 
    while (!pq.empty()) {

        // get the top element of 'pq' 
        TNode top = pq.top();
        pq.pop();
        if (ReadLine(top.FileNo, line)) {
            pq.push({ top.FileNo, line });
        }
        gzputs(OutputFile, top.Line.c_str());
        gzputs(OutputFile, "\n");
    }
    Close();
}