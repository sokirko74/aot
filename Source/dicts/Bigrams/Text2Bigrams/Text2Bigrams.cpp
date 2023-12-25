#include "graphan/GraphanLib/GraphmatFile.h"
#include "graphan/GraphanLib/graline.h"
#include "MergeFiles.h"

#include <string>
#include <list>
#include "morph_dict/common/argparse.h"


typedef std::map<std::pair<std::string,std::string>, size_t> BigramsType;
typedef std::list<std::vector<std::string> > interp_t;

struct CBigramsCreator {
    bool TakeOnlyWords;
    MorphLanguageEnum Langua;
    int MaxWindowSize;
    int MaxBigramsInMemory;
    std::string InputFileList;
    std::string  OutputFolder;
    
    std::map<std::string, size_t> WordFreqs;
    std::vector<std::string> AllTempFiles;
    BigramsType Bigrams;

    CBigramsCreator(MorphLanguageEnum l,  bool only_words, int max_window_wize, int max_memory,
                    std::string input_file_list,
                    std::string output_folder): 
        TakeOnlyWords(only_words), Langua(l), MaxWindowSize(max_window_wize), 
        MaxBigramsInMemory(max_memory),   InputFileList(input_file_list), OutputFolder(output_folder)
        {}; 
    void WriteBigramsToTempFile();
    void RemoveTempFiles();
    bool IsBigramToken(const CGraLine& token) const;
    interp_t GetTokensBySentences(CGraphmatFile& Graphan);
    void WriteUnigrams();
    void WriteBigrams();
    void BuildBigrams();
};


std::string	CreateTempFileName(std::string folder)
{
    int max_id = 0;
    for (const auto & entry : fs::directory_iterator(folder)) {
        std::string a = entry.path();
        if (startswith(a, "bigrams.") && endswith(a, ".tmp")) {
            auto items = split_string(a, '.');
            if (items.size() == 3 && atoi(items[1].c_str()) > 0) {
                auto id = atoi(items[1].c_str());
                if (max_id < id) {
                    max_id = id;
                }
            }
        }
    }
    auto filename = Format("bigrams.%i.tmp", max_id + 1);
    return fs::path(folder) / filename;
};

/*
 this function writes bigrams to the main file, bigrams can be reoccur in this file.
 It is the task of the following scripts to factorize this file by bigrams.
 if the input file is more than 2 Gb, the program starts writing another file
*/

void CBigramsCreator::WriteBigramsToTempFile()
{
    std::string path = CreateTempFileName(OutputFolder);
    LOGI << "write to temporary file " << path;
    AllTempFiles.push_back(path);
    
	FILE* big_fp = fopen (path.c_str(), "a");
    if (!big_fp)
    {
        throw CExpc("cannot write to %s, may be the file is too large",  path.c_str());
    };
	try {	
		for(auto& [key, value]: Bigrams)
		{
			fprintf(big_fp, "%s\t%s\t%zu\n", key.first.c_str(), key.second.c_str(), value);
		};
		fclose (big_fp);
	}
	catch (...)
	{
		LOGE <<  "cannot write to "<<  path << " , may be the file is too large";
        throw ;
	};
	Bigrams.clear();
};

void CBigramsCreator::RemoveTempFiles()
{
	for (auto& f : AllTempFiles) {
		remove(f.c_str());
	}
};


bool CBigramsCreator::IsBigramToken(const CGraLine& token) const
{
	if (TakeOnlyWords) {
		return token.IsWordOrNumberOrAbbr();
	}
	return !token.IsNotPrint()  && !token.IsSoft();
};

interp_t CBigramsCreator::GetTokensBySentences(CGraphmatFile& Graphan)
{
	interp_t result;
	size_t TokensCount = Graphan.GetUnits().size();
	
	for (size_t i = 0; i < TokensCount; ++i)
	{
		std::vector<std::string> sentence;
		size_t k = i;
		for (; k < TokensCount; ++k)
		{
			if (IsBigramToken(Graphan.GetUnits()[k]) )
			{
				std::string s = Graphan.GetTokenUtf8(k);
				if (!s.empty()) {
                    MakeUpperUtf8(s);
					sentence.push_back(s);
				}
			}

			if (Graphan.HasDescr(k, OSentEnd)) 
					break;
		}
		result.push_back(sentence);
		i = k;
	}

	return result;
};


std::vector<std::string> ReadListFile(std::string path) {
    std::ifstream inp(path);
    if (!inp.is_open()) {
        throw CExpc("cannot open file %s", path.c_str());
    }
    std::string line;
    std::vector<std::string> res;
    while (std::getline(inp, line)) {
        Trim(line);
        if (!fs::exists(line)) {
            throw CExpc("cannot read file %s", line.c_str());
        }
        res.push_back(line);
    }
    return res;
}

void CBigramsCreator::WriteUnigrams() {
    std::string path = fs::path(OutputFolder) / "unigrams.txt";
    LOGI << "Writing word frequences " << path;
    std::ofstream outp(path);
    if (!outp.is_open())
    {
        throw CExpc("Error! Cannot write  word frequences");
    }
    for (const auto a : WordFreqs)
    {
        outp << a.first << " " << a.second << "\n";
    }
}
void CBigramsCreator::WriteBigrams() {
    auto path = fs::path(OutputFolder) / "bigrams.txt.gz";
    LOGI << "writing bigrams to ", path;
    MergeSortedFiles m(AllTempFiles, path.string());
    m.Merge();
}

void CBigramsCreator::BuildBigrams()
{
	size_t AllFileSize = 0;

    CGraphmatFile Graphan;
    Graphan.LoadDicts(Langua);
	Graphan.m_bUseIndention = false;
	Graphan.m_MaxSentenceLength = 1000;
	StringVector files = ReadListFile(InputFileList);
    for (size_t file_no = 0; file_no < files.size(); file_no++) {
        std::string InputFileName = files[file_no];
        fprintf(stderr, "===== [%zu/%zu] %s ===== \n", file_no + 1, files.size(), InputFileName.c_str());
        AllFileSize += FileSize(InputFileName.c_str());
        Graphan.LoadFileToGraphan(InputFileName);
        interp_t sents = GetTokensBySentences(Graphan);

        for (auto &sent: sents) {
            int SentSize = sent.size();
            for (size_t token_no = 0; token_no < SentSize; token_no++) {
                std::string &s1 = sent[token_no];
                auto freq_it = WordFreqs.find(s1);
                if (freq_it == WordFreqs.end())
                    WordFreqs[s1] = 1;
                else
                    freq_it->second++;

                // add words from this window size 
                int WindowEnd = std::min(SentSize, (int) (MaxWindowSize + token_no + 1));
                for (size_t k = token_no + 1; k < WindowEnd; k++) {
                    std::string &s2 = sent[k];
                    auto p = std::make_pair(s1, s2);
                    auto it = Bigrams.find(p);
                    if (it != Bigrams.end())
                        it->second++;
                    else
                        Bigrams[p] = 1;
                };
            };
        }

        // spooling bigrams
        if (Bigrams.size() > MaxBigramsInMemory) {
            WriteBigramsToTempFile();
        };
    }
    
    WriteBigramsToTempFile();
    LOGI << "AllFilesSize = " << AllFileSize;
    WriteBigrams();
    RemoveTempFiles();
    WriteUnigrams();
    LOGI << "exiting";
};

void initArgParser(int argc, const char **argv, ArgumentParser& parser) {
    parser.AddOption("--help");
    parser.AddArgument("--language", "language");
    parser.AddArgument("--input-file-list", "test.con, each line contains a file");
    parser.AddArgument("--output-folder", "output folder");
    parser.AddArgument("--window-size", "collent bigrams in window, by default is 1", true);
    parser.AddArgument("--max-memory", "store not more than N ngrams in memory, by default 1000000", true);
    parser.AddArgument("--log-level", "log level", true);
    parser.AddOption("--only-words", "ignore puctuation marks");
    parser.Parse(argc, argv);
}


int main(int argc, const char* argv[])
{
    ArgumentParser args;
    initArgParser(argc, argv, args);
    init_plog(args.GetLogLevel(), "text2bigrams.log", true);

	std::string Error;
	if (!IsRmlRegistered(Error)) 
	{
		LOGE << Error;
		return 1;
	};
    try {
        int max_window_wize = args.Retrieve("window-size", 1);
        LOGI << "windows size is " << max_window_wize;

        int  MaxBigramsInMemory = args.Retrieve("max-memory", 1000000);
        LOGI << "max memory " << MaxBigramsInMemory;

        std::string output_folder = args.Retrieve("output-folder");
        LOGI << "output folder = " << output_folder;

        CBigramsCreator c(args.GetLanguage(), args.Exists("only-words"),
                          max_window_wize, MaxBigramsInMemory, args.Retrieve("input-file-list"),
                          output_folder);
        c.BuildBigrams();
        return 0;
    }
    catch (std::exception& c) {
        LOGE << "An exception occurred " << c.what();
        return 1;
    }
    catch (...)
    {
        LOGE << "An exception occurred!";
        return 1;

    };
}


