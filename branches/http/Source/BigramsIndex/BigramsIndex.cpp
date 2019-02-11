// BigramsIndex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../common/utilit.h"


void PrintUsageAndExit()
{
	fprintf (stderr,"BigramsIndex,  creates word-index for bigrams file\n");
	fprintf (stderr,"Usage: Bigrams <bigram-file> \n");
	exit(1);
	
};

struct CWordInfo
{
	string		m_WordStr;
	size_t		m_Id;
	size_t		m_Freq;

	size_t		m_FileOffset1;
	size_t		m_FileLen1;

	size_t		m_FileOffset2;
	size_t		m_FileLen2;
	bool operator < (const CWordInfo& W) const 
	{
		return m_WordStr < W.m_WordStr;
	}
	bool operator == (const CWordInfo& W) const 
	{
		return m_WordStr == W.m_WordStr;
	}
	bool operator < (const string& s) const 
	{
		return m_WordStr < s;
	}
	bool operator == (const string& s) const 
	{
		return m_WordStr == s;
	}

};

bool ReadWordFreqs(string WordFreqFileName, vector<CWordInfo>& WordInfos)
{
	fprintf (stderr, "open file %s\n", WordFreqFileName.c_str());
	FILE * freq_fp = fopen (WordFreqFileName.c_str(), "rb");
	if (!freq_fp)
	{
		fprintf (stderr, "cannot open file %s\n", WordFreqFileName.c_str());
		return false;
	}
	WordInfos.clear();
	char buffer[10000];
	while (fgets(buffer, 10000, freq_fp))
	{
		char w[500];
		int freq;
		if (sscanf(buffer, "%[^ ] %i", w, &freq) != 2)
		{
			fprintf (stderr, "%s: skip line %s", WordFreqFileName.c_str(), buffer);
			continue;
		}
		CWordInfo I;
		I.m_Freq = freq;
		I.m_FileOffset1 = UINT_MAX;
		I.m_FileOffset2 = UINT_MAX;
		I.m_FileLen1 = 0;
		I.m_FileLen2 = 0;
		I.m_WordStr = w;
		if (binary_search(WordInfos.begin(), WordInfos.end(), I))
		{
			fprintf (stderr, "A dublicate \"%s\" is found\n", w);
			return false;
		}
		if (!WordInfos.empty())
			if (!(WordInfos.back() < w))
			{
				fprintf (stderr, "Wrong  position  for word \"%s\" is found\n", w);
				return false;
			}
		WordInfos.push_back( I );
	}
	fprintf (stderr, "Number of read words: %u\n", WordInfos.size());

	return true;
}

bool BuildBigramsBin(string BigramsFileName, vector<CWordInfo>& WordInfos, bool bFirstOffset1)
{
	string WordFreqFileName =  MakeFName(BigramsFileName, "wrd_freq");
	fprintf (stderr, "open file %s\n", BigramsFileName.c_str());
	FILE * in_fp = fopen (BigramsFileName.c_str(), "rb");
	if (!in_fp)
	{
		fprintf (stderr, "cannot open file %s\n", BigramsFileName.c_str());
		return false;
	}

	string BinName = MakeFName(BigramsFileName, bFirstOffset1 ? "bin1" : "bin2");
	fprintf (stderr, "write  to  file %s\n", BinName.c_str());
	FILE * out_fp = fopen (BinName.c_str(),  "wb");
	if (!out_fp)
	{
		fprintf (stderr, "cannot open file %s\n", BinName.c_str());
		return false;
	}

	
	char buffer[10000];
	size_t BigramsCount = 0;
	vector<CWordInfo>::iterator curr_it = WordInfos.end();
	while (fgets(buffer, 10000, in_fp))
	{
		if ((BigramsCount % 100000) == 0)
			fprintf (stderr, "%u               \r", BigramsCount);
		char w1[500], w2[500];
		int Bigramsfreq;
		int Res = sscanf(buffer, "%[^\t]\t%[^\t]\t%i", w1, w2, &Bigramsfreq);
		if ( Res != 3)
		{
			fprintf (stderr, "%s: skip line %s (scanf returned %i)\n", BigramsFileName.c_str(), buffer, Res);
			continue;
		}
		const char* w  = bFirstOffset1 ? w1 : w2;
		const char* conv_w  = bFirstOffset1 ? w2 : w1;
		bool bChange = false;
		if	(		(curr_it == WordInfos.end())
				||	(w != curr_it->m_WordStr)
			)
		{
			if (curr_it == WordInfos.end())
				curr_it = lower_bound (WordInfos.begin(),  WordInfos.end(), w);
			else
			{
				curr_it++;
				curr_it = lower_bound (curr_it,  WordInfos.end(), w);
			}
			bChange = true;
		}

		if (curr_it == WordInfos.end())
		{
			fprintf (stderr, "Cannot find word \"%s\" in %s\n", w, buffer);
			return false;
		}
		else
		{
			if (bFirstOffset1)
			{
				if (bChange && (curr_it->m_FileOffset1 != UINT_MAX))
				{
					fprintf (stderr, "Wrong order \"%s\" from line \"%s\" (curr_it->m_FileOffset1=%u)\n", w, buffer, curr_it->m_FileOffset1);
					return  false;
				}

				if (curr_it->m_FileOffset1 == UINT_MAX)
					curr_it->m_FileOffset1 = BigramsCount;

				curr_it->m_FileLen1++;
			}
			else
			{
				if (bChange && (curr_it->m_FileOffset2 != UINT_MAX))
				{
					fprintf (stderr, "Wrong order \"%s\" from %s\n", w, buffer);
					return  false;
				}

				if (curr_it->m_FileOffset2 == UINT_MAX)
					curr_it->m_FileOffset2 = BigramsCount;

				curr_it->m_FileLen2++;
			}

			vector<CWordInfo>::const_iterator conv_curr_it = lower_bound(WordInfos.begin(), WordInfos.end(), conv_w);
			if (		conv_curr_it == WordInfos.end() 
					||	(conv_curr_it->m_WordStr != conv_w)
				)
			{
				fprintf (stderr, "Cannot find word \"%s\" in %s\n", conv_w, buffer);
				return false;
			}
			else
			{
				size_t ConvWordNo =  conv_curr_it - WordInfos.begin();
				// write the other item of the bigram
				fwrite(&ConvWordNo, 1, sizeof(ConvWordNo), out_fp);
				// write the frequence of bigram
				fwrite(&Bigramsfreq, 1, sizeof(Bigramsfreq), out_fp);
			}

		}

		BigramsCount++;
	}
	fprintf (stderr, "%u               \n", BigramsCount);
	fclose(in_fp);
	fclose (out_fp);
	return true;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		PrintUsageAndExit();

	vector<CWordInfo> WordInfos;
	string BigramsFileName =  argv[1];
	string WordFreqFileName =  MakeFName(BigramsFileName, "wrd_freq");

	// читаем статистику слов
	if (!ReadWordFreqs(WordFreqFileName, WordInfos))
		return 1;

	// создаем бинарный файл для биграмм
	if (!BuildBigramsBin(BigramsFileName, WordInfos, true))
		return 1;

	{
		string RevFile = MakeFName(BigramsFileName, "rev");
		string Command = Format ("gsort -k 2,3 <%s >%s", BigramsFileName.c_str(), RevFile.c_str());
		fprintf (stderr, "%s\n", Command.c_str());
		if (system (Command.c_str()) != 0)
		{
			fprintf (stderr,"!!! an exception occurred (cannot sort) !!!\n");
			return 1;
		};

		if (!BuildBigramsBin(RevFile, WordInfos, false))
			return 1;
		fprintf (stderr, "remove %s\n", RevFile.c_str());
		remove(RevFile.c_str());
	}


	string OutIndexFile = MakeFName(BigramsFileName, "wrd_idx");
	fprintf (stderr, "create file %s\n", OutIndexFile.c_str());
	FILE* fp = fopen (OutIndexFile.c_str(), "w");
	if (!fp)
	{
		fprintf (stderr, "cannot open file %s\n", OutIndexFile.c_str());
		return 1;
	}
	for (vector<CWordInfo>::const_iterator it = WordInfos.begin(); it != WordInfos.end(); it++)
	{
		fprintf (fp, "%s %u %u %u %u %u\n",it->m_WordStr.c_str(), 
			it->m_Freq, 
			it->m_FileOffset1,it->m_FileLen1,
			it->m_FileOffset2,it->m_FileLen2
			);
	}
	fclose (fp);
	return 0;
}





