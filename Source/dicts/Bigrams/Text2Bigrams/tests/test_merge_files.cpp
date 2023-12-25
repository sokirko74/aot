#include "../MergeFiles.h"
#include <streambuf>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "morph_dict/contrib/doctest/doctest.h"

TEST_CASE("test_merge_files")
{
    auto folder = fs::path(__FILE__).parent_path();
    auto outfile = "out.txt.gz";
    std::vector<std::string> input;
    for (auto& fiename: {"a1.txt", "a3.txt", "a2.txt"}) {
        auto p = (folder / fiename).string();
        input.push_back(p);
    }
    MergeSortedFiles m(input, outfile);
    m.Merge();
    gzFile file = gzopen(outfile, "rb");
    char buffer[1024];
    int cnt = 0;
    while (gzgets(file, buffer, sizeof(buffer)) != nullptr) {
        ++cnt;
        CHECK(cnt == atoi(buffer));
    }
    gzclose(file);
}

