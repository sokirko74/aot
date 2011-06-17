#include  "BigramsReader.h"
#include  "../common/bserialize.h"
#include  <climits>

struct CBigramsWordInfo
{
	string		m_Word;
	size_t		m_Freq;
	size_t		m_FileOffset1;
	size_t		m_FileLen1;
	size_t		m_FileOffset2;
	size_t		m_FileLen2;
	bool	operator == (const string & Word) const 
	{
		return m_Word == Word;
	}
};

struct CBigramAndFreq {
	string	m_Word;
	int		m_WordFreq;
	int		m_BigramsFreq;
};


class  CBigrams 
{
	vector<CBigramsWordInfo>		m_Word2Infos;
	FILE*							m_Bigrams;
	FILE*							m_BigramsRev;
	// пока вычисл€ем только контактные биграммы, 
	// m_CorpusSize вычисл€ютс€ суммированием всех биграмм минус кол-во предложений
	// (кол-во предложений можно опустить), а потом длину корпуса  
	// частоты отдельных слов лучше всего будет задавать  отдельно.
	

	public: 
	size_t							m_CorpusSize;
	CBigrams ();
	~CBigrams ();
	bool Initialize(string BigramsFileName);
	vector<CBigramAndFreq> GetBigrams(string Word, int MinBigramsFreq, bool bDirectFile, size_t& WordFreq);
};

static CBigrams	GlobalBigrams;

struct IsLessBigramsWordInfo 
{
		bool	operator ()  (const CBigramsWordInfo& X1, const string& Word) const 
		{
			return X1.m_Word < Word;
		};
		bool	operator ()  (const string & Word, const CBigramsWordInfo& X2 ) const 
		{
			return Word < X2.m_Word;
		};
		bool	operator ()  (const CBigramsWordInfo& X1, const CBigramsWordInfo& X2) const 
		{
			return X1.m_Word < X2.m_Word;
		};
};

CBigrams::CBigrams ()
{
	m_Bigrams = 0;
	m_BigramsRev = 0;

}

CBigrams::~CBigrams()
{
	if (m_Bigrams != 0) fclose (m_Bigrams);
	if (m_BigramsRev != 0) fclose (m_BigramsRev);
}



bool CBigrams::Initialize(string BigramsFileName)
{
	m_Word2Infos.clear();
	m_CorpusSize = 0;
	
	string IndexFile = MakeFName(BigramsFileName, "wrd_idx");
	fprintf (stderr,"load %s into memory\n", IndexFile.c_str()  );
	FILE *fp = 	fopen (IndexFile.c_str(), "r");
	if (!fp)
	{
		fprintf (stderr,"Cannot open %s", IndexFile.c_str());
		return false;
	}
	char buffer[1000];
	while (fgets (buffer, 1000, fp))
	{
		char word[1000];
		CBigramsWordInfo I;
		if (sscanf(buffer, "%[^ ] %u %u %u %u %u", word, 
			&I.m_Freq, 
			&I.m_FileOffset1, &I.m_FileLen1,
			&I.m_FileOffset2, &I.m_FileLen2) != 6)
		{
			fprintf (stderr,"Bad format in  %s", IndexFile.c_str());
			fclose(fp);
			return false;
		}
		I.m_Word = word;
		if (lower_bound(m_Word2Infos.begin(),  m_Word2Infos.end(), word, IsLessBigramsWordInfo()) != m_Word2Infos.end() )
		{
			fprintf (stderr,"A dublicate \"%s\" is found", word);
			fclose(fp);
			return false;
		}
		m_Word2Infos.push_back( I );
		m_CorpusSize += I.m_Freq;
	}
	fclose(fp);
	fprintf (stderr,"  open %s \n", BigramsFileName.c_str()  );
	if (m_Bigrams) fclose (m_Bigrams);

	string Bin1File = MakeFName(BigramsFileName, "bin1");
	fprintf (stderr, "  open %s \n", Bin1File.c_str()  );
	m_Bigrams = fopen (Bin1File.c_str(), "rb");
	if (!m_Bigrams)
	{
		fprintf (stderr, "Cannot open file %s", Bin1File.c_str());
		return false;
	}

	
	if (m_BigramsRev) fclose (m_BigramsRev);
	string Bin2File = MakeFName(BigramsFileName, "bin2");
	fprintf (stderr, "  open %s \n", Bin2File.c_str()  );
	m_BigramsRev = fopen (Bin2File.c_str(), "rb");
	if (!m_BigramsRev)
	{
		fprintf (stderr, "Cannot open file %s", Bin2File.c_str());
		return false;
	}

	return true;
}



vector<CBigramAndFreq> CBigrams::GetBigrams(string Word, int MinBigramsFreq, bool bDirectFile, size_t& WordFreq)
{
	WordFreq = 0;
	vector<CBigramAndFreq> Result;
	if (!m_Bigrams) return Result;
	if (!m_BigramsRev) return Result;
	EngRusMakeUpper(Word);
	// find word in the index
	vector<CBigramsWordInfo>::const_iterator it = lower_bound(m_Word2Infos.begin(), m_Word2Infos.end(),Word, IsLessBigramsWordInfo());
	if (it == m_Word2Infos.end()) return Result;

	const CBigramsWordInfo& WordInfo = *it;

	// in the bigrams file a sequence  of integer pair is written:
	// the first element is if of the other word of the bigrams
	// the second element is the bigram frequence
	size_t  fpos = (bDirectFile ? WordInfo.m_FileOffset1:WordInfo.m_FileOffset2);
	if (fpos == UINT_MAX) 
		return Result;
	size_t  size = bDirectFile ? WordInfo.m_FileLen1:WordInfo.m_FileLen2;
	FILE * big_fp = bDirectFile ? m_Bigrams : m_BigramsRev;

	if (fseek(big_fp, fpos*sizeof(pair<int,int>), SEEK_SET))
	{
		return Result;
	}
	vector<pair<int,int> > Bigrams;
	ReadVectorInner(big_fp,  Bigrams, size);
	

	// swaping bigram frequence and the second element in order to sort by frequence
	for(int i=size-1; i >=0; i--)
		if (Bigrams[i].second < MinBigramsFreq )
			Bigrams.erase(Bigrams.begin() + i);
		else
			swap(Bigrams[i].second,Bigrams[i].first);
	
	// triming bigrams if there are to many of them 
	WordFreq =  WordInfo.m_Freq;
	if (Bigrams.size() > 250)
	{
		sort (Bigrams.begin(),  Bigrams.end());
		Bigrams.erase(Bigrams.begin(), Bigrams.begin() +  Bigrams.size() - 250);
	}

	
	for (size_t  i =0; i < Bigrams.size(); i++)
	{
		const CBigramsWordInfo& ConvWord = m_Word2Infos[Bigrams[i].second];
		// writing word, word frequence,  bigram frequence
		//Result +=  Format("%s_%u %u %u\n", ConvWord.m_Word.c_str(), Bigrams[i].second, ConvWord.m_Freq, Bigrams[i].first);
		CBigramAndFreq B;
		B.m_Word = ConvWord.m_Word;
		B.m_BigramsFreq = Bigrams[i].first;
		B.m_WordFreq = ConvWord.m_Freq ;
		
		Result.push_back(B);
	};
	return  Result;
}

string GetConnectedWords(string Word, int MinBigramsFreq, bool bDirectFile)
{
	size_t WordFreq;
	vector<CBigramAndFreq> R = GlobalBigrams.GetBigrams(Word,  MinBigramsFreq, bDirectFile, WordFreq);
	// dumping bigrams

	string Result = Format("%u %u %s\n", GlobalBigrams.m_CorpusSize, WordFreq, Word.c_str());
	for (size_t  i =0; i < R.size(); i++)
	{
		Result +=  Format("%s %u %u\n", R[i].m_Word.c_str(), R[i].m_WordFreq, R[i].m_BigramsFreq);
	};
	return Result;
}

string GetBigramsAsString(string Word, int MinBigramsFreq, bool bDirectFile)
{
	size_t WordFreq;
	vector<CBigramAndFreq> R = GlobalBigrams.GetBigrams(Word,  MinBigramsFreq, bDirectFile, WordFreq);
	// dumping bigrams

	string Result;
	for (size_t  i =0; i < R.size(); i++)
	{
		Result +=  Format("%s\t%s\t%u\n", Word.c_str(), R[i].m_Word.c_str(),  R[i].m_BigramsFreq);
	};
	return Result;
}

bool InitializeBigrams(string FileName)
{
	return GlobalBigrams.Initialize(FileName);

}