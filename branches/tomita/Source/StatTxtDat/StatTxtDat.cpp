//////////////////////////////////////////////////////////////////////////////
// StatTxtDat <InListFile> <[DataFile="StatData.txt"]>
// Только для РУССКОГО ЯЗЫКА - использует SynAn
// Подгружает данные из файла <DataFile> если может
// Проходит по всем файлам,указанным построчно в <InListFile>
// Создает и записывает в файл <DataFile> статистическую информацию
//  для дальнейшей ручной правки
// Вид файла <DataFile>:
// МЫЛА              МЫЛО              С ср,ед,им,но,        1
// *                 МЫТЬ              ИНФИНИТИВ нс,пе,дст,  2
// Результат затем может подаваться в StatDatBin

// Если существуют такие омонимы Хi и Xj, что (V(Xi)/V(Xj)) > 50, тогда
//  объявить, что вся группа омонимов считается "устоявшейся", что значит,
//  что при новых итерациях программы StatTxtDat частоты V(X1),...,V(Xn) 
//  уже не меняются.

//////////////////////////////////////////////////////////////////////////////
#include "..\common\utilit.h"
#include <cassert>
#include <iostream>
#include <fstream>
#include <strstream>
#include <string>
#include <map>

#include "../common/COMSyntaxHolder.h"

struct CGroup
{
	int pid;
	std::string anc;

	CGroup() : pid(0),anc("") {}

	friend bool operator<(const CGroup &x,const CGroup &y);
};

bool operator<(const CGroup &x,const CGroup &y)
{
	if( x.anc<y.anc )
		return( 1 );
	if( x.anc>y.anc )
		return( 0 );
	return( x.pid<y.pid );
}

typedef std::map<CGroup,int> tagLines;
typedef std::map<std::string,tagLines> tagHomon; 
static tagHomon homon;

//////////////////////////////////////////////////////////////////////////////
// Msg/Err

static inline std::string msg_usg() 
{ 
	return "Usage: StatTxtDat <Language> <InListFile> <[DataFile=\"StatData.txt\"]>"
			"Where Language can be \"Russian\" or \"German\"\n"
			"Example StatTxtDat  Russian FileList.txt StatData.txt\n"
		   ;
}
static inline std::string err_opn() 
{ 
	return "File open error: "; 
}

//////////////////////////////////////////////////////////////////////////////

static CCOMSyntaxHolder Holder;
static MorphLanguageEnum Language;

//////////////////////////////////////////////////////////////////////////////

static bool initCom()
{
	CoInitialize(NULL);
	return Holder.LoadSyntax(Language) == TRUE;
}

static void exitCom()
{
	Holder.DeleteProcessors();
	CoUninitialize();
}

//////////////////////////////////////////////////////////////////////////////

static void addLines(std::string &str,tagLines &lines)
{
	assert( !str.empty() && lines.size()>0 );
	std::pair<tagHomon::iterator,bool> res;
	res = homon.insert(tagHomon::value_type(str,lines));
	if( !res.second )
	{
		char oem[256];
		CharToOem(str.c_str(),oem);
		std::cout << "Duplicate word: " << oem << " skipped" << std::endl;
	}
}

static bool loadDat(std::string &name)
{
	std::ifstream ifs(name.c_str());
	if( !ifs.is_open() )
		return true;

	std::cout << "Loading " << name.c_str() << "... ";

	std::string str;
	tagLines lines;

	char buf[2048];
	int lin = 0;

	while( ifs.getline(buf,2048) )
	{
		lin++;
		std::istrstream istr(buf);
		std::string swrd,spid,spos,sgrm;
		int val=-1;

		istr >> swrd >> spid >> spos >> sgrm >> val;
		if( swrd.empty() || spid.empty() || spos.empty() )
		{
			std::cout << "Error in line: " << lin << " skipped" << std::endl;
			continue;
		}

		if( val<0 && atoi(sgrm.c_str())>=0 )
		{
			val = atoi(sgrm.c_str());
			sgrm.erase();
		}
//
		std::string grm = spos;
		grm += " ";
		grm += sgrm;
		BYTE pos;
		QWORD gra;
		BSTR code;
		if( !Holder.m_pGramTab->ProcessPOSAndGrammemsIfCan(grm.c_str(),&pos,&gra) ||
			!Holder.m_pGramTab->GetGramCodeByGrammemsAndPartofSpeechIfCan(pos,gra,&code) )
		{
			std::cout << "Error in line: " << lin << " skipped" << std::endl;
			continue;
		}
		std::string def = _bstr_t(code);
//
		if( str.empty() && swrd == "*" )
			continue;
		if( swrd != "*" )
		{
			if( !str.empty() && lines.size()>0 )
				addLines(str,lines);
			str = swrd;
			lines.clear();
		}

		LEMMATIZERLib::IParadigmCollectionPtr piPC;
		
		piPC = Holder.m_piLemmatizer->CreateParadigmCollectionFromNorm(spid.c_str(),true, false);
		int k=0;
		for( ; k<piPC->GetCount(); k++ )
		{
			LEMMATIZERLib::IParadigmPtr piPagadigm;
			piPagadigm = piPC->GetItem(k);
			CGroup group;
			group.pid = piPagadigm->GetParadigmID();
			group.anc = piPagadigm->GetAncode(0);
			group.anc = group.anc.substr(0,2);
			if( group.anc == def )
			{
				std::pair<tagLines::iterator,bool> res;
				res = lines.insert(tagLines::value_type(group,val));
				if( !res.second )
					std::cout << "Duplicate pid/anc: " << lin << " skipped" << std::endl;
				break;
			}
		}
		if( k>=piPC->GetCount() )
		{
			std::cout << "Can't calculate pid/anc ! LineNo= " << lin << " Operation aborted" << std::endl;
			return false;
		}
	}
//
	if( !str.empty() && lines.size()>0 )
		addLines(str,lines);
	ifs.close();
	std::cout << "done" << std::endl;
	return true;
}

//////////////////////////////////////////////////////////////////////////////

static bool saveDat(std::string &name)
{
	std::ofstream ofs(name.c_str());
	if( !ofs.is_open() )
	{
		std::cout << err_opn << name << std::endl;
		return( false );
	}

	std::cout << "Saving " << name.c_str() << "... ";
	tagHomon::iterator it;
	for( it=homon.begin(); it!=homon.end(); it++ )
	{
		char tmp[256];
		wsprintf(tmp,"%-16s  ",it->first.c_str());
		ofs << tmp;
		tagLines::iterator lin = it->second.begin();
		int pid = lin->first.pid;
		LEMMATIZERLib::IParadigmPtr piParadigm;
		piParadigm = Holder.m_piLemmatizer->CreateParadigmFromID(pid);
		std::string prd = piParadigm->GetNorm();
		wsprintf(tmp,"%-16s  ",prd.c_str());
		ofs << tmp;
		std::string anc = lin->first.anc;
		std::string grm;
		grm = Holder.m_pGramTab->GetPartOfSpeechStr(Holder.m_pGramTab->GetPartOfSpeech(anc.c_str()));
		grm += " ";
		grm += Holder.m_pGramTab->GrammemsToStr(Holder.m_pGramTab->GetGrammems(anc.substr(0,2).c_str()));
		wsprintf(tmp,"%-20s  ",grm.c_str());
		ofs << tmp;
		ofs << lin->second << std::endl;

		for( lin++; lin!=it->second.end(); lin++ )
		{
			wsprintf(tmp,"%-16s  ","*");
			ofs << tmp;
			LEMMATIZERLib::IParadigmPtr piParadigm;
			piParadigm = Holder.m_piLemmatizer->CreateParadigmFromID(lin->first.pid);
			prd = piParadigm->GetNorm();
			wsprintf(tmp,"%-16s  ",prd.c_str());
			ofs << tmp;
			anc = lin->first.anc;
			grm = Holder.m_pGramTab->GetPartOfSpeechStr(Holder.m_pGramTab->GetPartOfSpeech(anc.c_str()));
			grm += " ";
			grm += Holder.m_pGramTab->GrammemsToStr(Holder.m_pGramTab->GetGrammems(anc.substr(0,2).c_str()));
			wsprintf(tmp,"%-20s  ",grm.c_str());
			ofs << tmp;
			ofs << lin->second << std::endl;
		}
	}
	ofs.close();
	std::cout << "done" << std::endl;
	return true;
}

//////////////////////////////////////////////////////////////////////////////

static void workSent(SYNANLib::ISentencePtr piSentence)
{
	tagLines lines;
	int iWord=0,iClau=0,iCvar=0;

	try {
	
	int nWord = piSentence->GetWordsNum();
	for( iWord=0; iWord<nWord; iWord++ )
	{
		SYNANLib::IWordPtr piWord = piSentence->Word[iWord];
		std::string str = piWord->GetWordStr();
		if (!CheckLanguage(str, Language)) continue;
		if ( piWord->HomonymsCount<2 ) continue;
		RmlMakeUpper(str, Language);
		lines.clear();
		
		iClau = piWord->GetClauseNo();
		SYNANLib::IClausePtr piClause = piSentence->GetClause(iClau);
		int nCvar = piClause->GetVariantsCount();
		if( nCvar==0 )
		{
			std::cout << " GetVariantsCount() equal zero" << std::endl;
			break;
		}
		int nVmax = piClause->ClauseVariant[0]->VariantWeight;
		for( iCvar=0; iCvar<nCvar; iCvar++ )
		{
			int nVcur = piClause->ClauseVariant[iCvar]->VariantWeight;
//			if( nVcur<nVmax )
//				break;
			SYNANLib::IHomonymPtr piHomonym;
			piHomonym = piSentence->GetHomonymByClauseVar(iWord,iCvar,iClau);
			std::string tmp = piHomonym->GetLemma();
				
			CGroup group;
			group.pid = piHomonym->ParadigmID;
			if( group.pid==-1 )
				continue;

			LEMMATIZERLib::IParadigmPtr piParadigm;
			piParadigm = Holder.m_piLemmatizer->CreateParadigmFromID(group.pid);
			group.anc = piParadigm->GetAncode(0);
			group.anc = group.anc.substr(0,2);
// FoolProof
			int nCnt = piParadigm->GetCount();
			int iCnt=0;
			for( ; iCnt<nCnt; iCnt++ )
			{
				std::string form = piParadigm->GetForm(iCnt);
				if( form==str )
					break;
			}
			if( iCnt>=nCnt )
				continue;
//
			if( nVcur==nVmax )
				lines.insert(tagLines::value_type(group,1));
			else
				lines.insert(tagLines::value_type(group,0));
		} // nCvar

		if( lines.empty() )
			continue;

		std::pair<tagHomon::iterator,bool> res;
		res = homon.insert(tagHomon::value_type(str,lines));
		if( res.second )
			continue;
		tagLines& ori = res.first->second;
		tagLines::iterator it;
// вот здесь проверим, не является ли группа омонимов "устоявшейся"
		int vMin = ori.begin()->second;
		int vMax = ori.begin()->second;
		for( it=ori.begin(); it!=ori.end(); it++ )
		{
			if( vMin > it->second )
				vMin = it->second;
			if( vMax < it->second )
				vMax = it->second;
		}
		if( vMin==0 )
			vMin = 1;
		if( vMax/vMin > 50 )
			continue;
//		
		for( it=lines.begin(); it!=lines.end(); it++ )
		{
			std::pair<tagLines::iterator,bool> lin;
			lin = ori.insert(*it);
			if( !lin.second )
				lin.first->second += it->second;
		}
	} // nWord

	} catch(...)
	{
		std::cout << " Error! word: " << iWord << " clause " << iClau << " var: " << iCvar << " - skip rest" << std::endl;
	}
}

//////////////////////////////////////////////////////////////////////////////

static void workFile(const char *name)
{
	Holder.GetSentencesFromSynAn(name, TRUE, FALSE, FALSE);

	int nSent = Holder.m_piSentCollection->SentencesCount;
	for( int iSent=0; iSent<nSent; iSent++ )
	{
		std::cout << "\r" << iSent+1;
		SYNANLib::ISentencePtr piSentence = Holder.m_piSentCollection->Sentence[iSent];
		workSent(piSentence);
	}

	std::cout << std::endl;
}

//////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	if( argc<3 )
	{
		std::cout << msg_usg() << std::endl;
		return 0;
	}
	std::string name;
	if( argc>3 )
		name = argv[3];
	else
		name = "StatData.txt";
	//
	if (!GetLanguageByString(argv[1], Language))
	{
		std::cout << msg_usg() << std::endl;
		return 1;
	};

	if( !initCom() )
		return 1;

	try {
		//
		if( !loadDat(name) )
			goto END;
		//
		std::ifstream ifs(argv[2]);
		if( !ifs.is_open() )
		{
			std::cout << err_opn() << argv[2] << std::endl;
			goto END;
		}
		char iname[256];
		while( ifs.getline(iname,256) )
			workFile(iname);
		ifs.close();
		//
		saveDat(name);
		exitCom();
		return 0;

	} catch(...)
	{
		std::cout << std::endl << "An error in try{} has occurred!" << std::endl;
	}

END:;	
	exitCom();
	return 1;
}

//////////////////////////////////////////////////////////////////////////////
