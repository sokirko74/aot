//////////////////////////////////////////////////////////////////////////////
// WordDatBin <InDataFile> <-F | -C | -L> [Output Directory]
// Подгружает данные из файла <DataFile> (см. StatTxtDat)
// Записывает пары <pid,weight> в файл "r*wordweight.bin"

//////////////////////////////////////////////////////////////////////////////
#pragma warning(disable: 4786)
#include "../common/util_classes.h"
#include "../common/bserialize.h"

#include <algorithm>
#include <iostream>
#include <fstream>

#include <string>
#include <map>
#include <vector>
#include "../common/MorphologyHolder.h"


CMorphologyHolder MorphHolderRus;

static std::vector<std::pair<int, int> > ww;

//////////////////////////////////////////////////////////////////////////////
// Msg/Err

static inline std::string msg_usg() 
{
	return "Usage: WordDatBin <InDataFile> <-F | -C | -L> [Output Directory]\n"
		   "Example: WordDatBin WordData.txt -L RUS_BIN\\\n";
}

static inline std::string err_opn() { return "File open error: "; }

//////////////////////////////////////////////////////////////////////////////



//////////////////////////////////////////////////////////////////////////////
struct CStatInfo {
	string  m_Lemma;
	BYTE	m_Pos;
	QWORD	m_Grammems;
	int		m_Freq;
	bool	operator <(const CStatInfo& s) const
	{
		if (m_Lemma != s.m_Lemma)
			return m_Lemma < s.m_Lemma;
		else
		if (m_Pos != s.m_Pos)
			return m_Pos < s.m_Pos;
		else
			return m_Grammems < s.m_Grammems;
	}

	string DumpToString() const 
	{
		string Result = m_Lemma + ";";  
		Result += MorphHolderRus.m_pGramTab->GetPartOfSpeechStr(m_Pos);
		Result += " ";
		if (m_Grammems != 0)
			Result += MorphHolderRus.m_pGramTab->GrammemsToStr(m_Grammems);
		Result += Format(";%i", m_Freq);
		return Result;
	}
};

bool readDat(char *name, set<CStatInfo>& Infos)
{
	Infos.clear();
	std::ifstream ifs(name);
	if( !ifs.is_open() )
	{
		std::cout << err_opn() << name << std::endl;
		return false;
	}
//
	std::cout << "Loading " << name << "... " << std::endl;

	char buf[256];
	int lin = 0;

	while( ifs.getline(buf,2048) )
	{
		lin++;
		StringTokenizer tok(buf, ";");
		CStatInfo I;
		I.m_Lemma = tok();
		std::string sgrm = tok();
		I.m_Freq= atoi(tok());

		if( I.m_Lemma.empty() || sgrm.empty() )
		{
			std::cout << "Error in line: " << buf << " skipped" << std::endl;
			continue;
		};

		I.m_Pos = 1;
		I.m_Grammems = 1;
		if (!MorphHolderRus.m_pGramTab->ProcessPOSAndGrammemsIfCan(sgrm.c_str(), &I.m_Pos, &I.m_Grammems))
		{
			std::cout << "Error in morph. pattern line: " << buf << " skipped" << std::endl;
			continue;
		};
		pair <set<CStatInfo>::iterator, bool> r = Infos.insert(I);
		if ( ! r.second ) 
		{
			CStatInfo A = *r.first;;
			A.m_Freq += I.m_Freq;
			Infos.erase(r.first);
			Infos.insert(A);
		}

				
	}

	ifs.close();
	return true;
}

void DealWithUniqueLemmas (set<CStatInfo>& Infos)
{
	set<CStatInfo> NewInfos;
	for (set<CStatInfo>::iterator it = Infos.begin();it != Infos.end(); )
	{
		CStatInfo I  = *it;
		vector<CFormInfo> ParadigmCollection;

		string s = I.m_Lemma;
		if (!MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(true, s, true, false, ParadigmCollection))
		{
			it++;
			continue;
		}

		string CommonAncode;
		int CountWithTheSamePOS = 0;
		for( int k=0; k<ParadigmCollection.size(); k++ )
		{
			const CFormInfo& Pagadigm = ParadigmCollection[k];
			std::string anc = Pagadigm.GetAncode(0);
			if (I.m_Pos != MorphHolderRus.m_pGramTab->GetPartOfSpeech(anc.c_str())) continue;
			CommonAncode = Pagadigm.GetCommonAncode();
			CountWithTheSamePOS++;
		}

		if ( CountWithTheSamePOS == 1 )
		{
			set<CStatInfo>::iterator it1 = it;
			it1++;
			for (; it1 != Infos.end(); it1++)
				if (		(it1->m_Lemma == I.m_Lemma)
						&&	(it1->m_Pos == I.m_Pos)
					)
				{
					I.m_Freq += it1->m_Freq;
				}
				else
					break;
			
			I.m_Grammems = MorphHolderRus.m_pGramTab->GetAllGrammems(CommonAncode.c_str());
			it = it1;
			NewInfos.insert(I);
		}
		else
		{
			NewInfos.insert(I);
			it++;
		}
		
	}
	Infos.swap(NewInfos);

}
static bool loadDat(char *name)
{
	std::ofstream good_ofs(MakeFName(name,"good").c_str());
	set<CStatInfo> Infos;
	if (!readDat(name, Infos)) 
		return false;

	DealWithUniqueLemmas(Infos);

	for (set<CStatInfo>::const_iterator it = Infos.begin();it != Infos.end(); it++)
	{
		const CStatInfo& I  = *it;
		bool bFound = false;
		vector<CFormInfo> ParadigmCollection;
		string s = I.m_Lemma;
		if (!MorphHolderRus.m_pLemmatizer->CreateParadigmCollection(true, s, true, false, ParadigmCollection))
		{
			std::cout << "Exception in Lemmatizer (probably wrong ABC: " << I.m_Lemma << " skipped" << std::endl;
			continue;
		}
		
		for( int k=0; k<ParadigmCollection.size(); k++ )
		{
			const CFormInfo& Pagadigm = ParadigmCollection[k];
			std::string common_anc = Pagadigm.GetCommonAncode();
			std::string anc = Pagadigm.GetAncode(0);
			if (I.m_Pos != MorphHolderRus.m_pGramTab->GetPartOfSpeech(anc.c_str())) continue;
			if (I.m_Grammems != 0)
			{
				QWORD g = MorphHolderRus.m_pGramTab->GetAllGrammems(common_anc.c_str()) |  MorphHolderRus.m_pGramTab->GetAllGrammems(anc.c_str());
				if ((g &  I.m_Grammems) != I.m_Grammems)
						continue;
			}

			ww.push_back(  std::make_pair(Pagadigm.GetParadigmId(),I.m_Freq) );
			bFound = true;
		}
		if( !bFound )
		{
			std::cout << "Can't produce PID: " << I.DumpToString() << " skipped" << std::endl;
			continue;
		}
		good_ofs << I.DumpToString() << endl;
	}

//
	good_ofs.close();
	return true;
}

//////////////////////////////////////////////////////////////////////////////

static bool saveBin(std::string &name)
{
	std::sort(ww.begin(),ww.end());
	return WriteVector(name, ww);
}

//////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	int ret = 1;
	if( argc<3 )
	{
		std::cout << msg_usg() << std::endl;
		return 0;
	}

	std::string name;
	if( argc >= 4 )
		name = argv[3];

	if( strlen(argv[2])<2 )
	{
		std::cout << msg_usg() << std::endl;
		return 1;
	}
	if( argv[2][1]=='F' || argv[2][1]=='f' )
		name += "f";
	else if( argv[2][1]=='C' || argv[2][1]=='c' )
		name += "c";
	else if( argv[2][1]=='L' || argv[2][1]=='l' )
		name += "l";
	else
	{
		std::cout << msg_usg() << std::endl;
		return 1;
	}

	name += "wordweight.bin";
	//
	if (!MorphHolderRus.LoadGraphanAndLemmatizer(morphRussian))
	{
		fprintf(stderr, "cannot load Russian morphology\n");
		return 1;
	}

	try {
		//
		if( loadDat(argv[1]) )
			if( saveBin(name) )
				return 0;

	} 
	catch (CExpc e)
	{
		fprintf (stderr, "exception occurred: %s!\n", e.m_strCause.c_str());
	}
	catch(...)
	{
		std::cout << std::endl << "An error in try{} has occurred!" << std::endl;
		
	}

	return 1;
}

//////////////////////////////////////////////////////////////////////////////

