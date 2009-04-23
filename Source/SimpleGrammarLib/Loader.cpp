
#pragma warning(disable:4786)
#pragma warning(disable:4530)


#include  "SimpleGrammarLib.h"
#include <time.h>
#include "SimpleGrammar.h"
//#include "../common/DwdsThesaurus.h"
#include <sys/stat.h>
#include "../common/bserialize.h"

//========================================================================
//=====================   CWorkGrammar      ==============================
//========================================================================
CWorkGrammar::CWorkGrammar()
{
	m_GLRTable.m_pWorkGrammar  = this;
	m_bEnableRootPrefix	= true;
};
CWorkGrammar::~CWorkGrammar()
{
	
};

size_t		CWorkGrammar::GetItemId(const CGrammarItem& I)
{
	vector<CGrammarItem>::const_iterator it = find(m_UniqueGrammarItems.begin(),m_UniqueGrammarItems.end(),I);

	if (it != m_UniqueGrammarItems.end())
		return it - m_UniqueGrammarItems.begin();
	else
	{
		m_UniqueGrammarItems.push_back(I);
		// slot m_bSynMain is no used in  WorkGrammar it is used only in class CParseGrammar
		m_UniqueGrammarItems.back().m_bSynMain = false;
		return m_UniqueGrammarItems.size() - 1;

	};
};


string CWorkGrammar::GetLeftPartStr(set<CWorkRule>::const_iterator it) const
{
	return m_UniqueGrammarItems[it->m_LeftPart].GetDumpString();
};


string	CWorkGrammar::GetRuleStr(const CWorkRule& R, int AsteriskNo, bool bPrintFeatures) const
{
	string Result = m_UniqueGrammarItems[R.m_LeftPart].GetDumpString() + "->";
	size_t l=0;
	for (; l<R.m_RightPart.m_Items.size(); l++)
	{
		if (AsteriskNo == l)
			Result += "*";

		Result += m_UniqueGrammarItems[R.m_RightPart.m_Items[l]].GetDumpString();

		if (l+1 !=  R.m_RightPart.m_Items.size())
			Result += ", ";
	};
	if (AsteriskNo == l)
			Result += "*";

	if (bPrintFeatures)
	{
		const vector<CRuleFeature>&  Features = m_RuleFeatures[R.m_RuleFeaturesNo];
		for (size_t i=0; i < Features.size(); i++)
		{
			const CRuleFeature&  F = Features[i];
			Result += "\n\t\t : ";
			if (F.m_Type == foeAssignement)
			{
				Result += F.m_LeftItem.GetStr();
				Result += " := ";
			}
			Result += F.m_FuncNameStr;
			Result += "( ";
			for (size_t j=0; j < F.m_RightItems.size(); j++)
			{
				Result += F.m_RightItems[j].GetStr();

				if (j+1 != F.m_RightItems.size())
					Result += ", ";
			};
			Result += ") ";
		};
	};


	return Result;
};


string	CWorkGrammar::GetRuleStr(const CWorkRule& R) const
{
	return  GetRuleStr(R, R.m_RightPart.m_SynMainItemNo);
};

string	CWorkGrammar::GetRuleStr(CWRI it) const
{
	return GetRuleStr(*it);
	
};


void CWorkGrammar::Print() const
{
	//FILE* fp = fopen("log","a");
	printf ("===========\n");
	//fprintf (fp, "===========\n");
	int RuleNo = 0;
	for (CWRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
	{
		RuleNo++;
		printf ("%i) %s\n", it->m_RuleId, GetRuleStr(it).c_str());
		//fprintf (fp, "%i: %s\n", RuleNo, GetRuleStr(it).c_str());
	};
	//fclose (fp);
};




void	CWorkGrammar::AddToken(CTokenItem I)
{
	assert (!I.m_TokenStr.empty());
	GerMakeUpper(I.m_TokenStr);

	I.m_bPunctuation = (ispunct((BYTE)I.m_TokenStr[0]) != 0);

	m_TokenList.push_back(I);
};



#ifdef WIN32

size_t GetFileModifTime (const char* FileName)
{
   struct _stat buf;
   FILE* fp = fopen (FileName, "r");
   _fstat( fileno(fp), &buf ); 
   fclose (fp);
   return buf.st_mtime;
};
#else

size_t GetFileModifTime (const char* FileName)
{
   struct stat buf;
   FILE* fp = fopen (FileName, "r");
   fstat( fileno(fp), &buf ); 
   fclose (fp);
   return buf.st_mtime;
};

#endif



int CWorkGrammar::FindTokenListByFileName(const string& FileName, size_t EndItemNo)
{
	for (int i=0; i<EndItemNo; i++)
	{	
		string CurrFileName = m_UniqueGrammarItems[i].GetFullFileName(m_SourceGrammarFile);
		if (CurrFileName == FileName)
			return i;
	};
	return -1;
};


bool CWorkGrammar::LoadOptions() 
{
	m_SecondPassSymbols.clear();
	string FileName = MakeFName(m_SourceGrammarFile,"opt");
	fprintf (stderr, "loading %s\n",FileName.c_str());
	FILE* fp = fopen(FileName.c_str(), "r");
	if (!fp)
	{
		ErrorMessage( Format("cannot open file %s\n",FileName.c_str()));
		return false;
	};

	char buffer[1000];
	while (fgets(buffer, 1000, fp))
	{
		string Line = buffer;
		size_t i = Line.find("%%");
		if (i != string::npos)
			Line.erase(i);
		Trim(Line);
		if (Line.empty()) continue;
		StringTokenizer tok(Line.c_str(), "\t ");
		tok();
		string OptName = tok.val();
		if (OptName == "SecondPass")
			while (tok())
				m_SecondPassSymbols.insert(tok.val());
		else
			if (OptName == "DisableRootPrefix")
			{
				fprintf (stderr, "DisableRootPrefix\n");
				m_bEnableRootPrefix = false;
			}
			else
			{
				ErrorMessage(Format( "Unknown option in %s: %s \n", FileName.c_str(), Line.c_str()));
				fclose(fp);
				return false;
			};

	}
	fclose(fp);
	fprintf (stderr, "Number of second pass symbols : %i\n",m_SecondPassSymbols.size());
	return true;
};

bool CWorkGrammar::CreateTokenList(string& ErrorStr) 
{

	//FILE * tokens = fopen ("tokens_dump.txt", "w");
	for (size_t i=0; i<m_UniqueGrammarItems.size(); i++)
	{
		CGrammarItem& I = m_UniqueGrammarItems[i];
		string FileName =  I.GetFullFileName(m_SourceGrammarFile);
		if (!FileName.empty()) 
		{
			if (!FileExists(FileName.c_str()))
			{
				ErrorMessage(Format("cannot access %s\n",FileName.c_str()));
				return false;
			};

			int  PrevTokList  = FindTokenListByFileName(FileName, i);
			if (PrevTokList != -1)
			{
				// this list was already loaded, we can use one more time withoutz loading another copy
				I.m_pListFile = m_UniqueGrammarItems[PrevTokList].m_pListFile;
				assert (I.m_pListFile != NULL);
				continue;
			};
			
			{
				size_t ModifTime = GetFileModifTime(FileName.c_str());
				if ((I.m_pListFile != NULL) && (I.m_pListFile->m_SavedModifTime == ModifTime) )
					continue;
				else
					if (I.m_pListFile != NULL)
						I.m_pListFile->m_SavedModifTime = ModifTime;
					else
					{
						CTokenListFile F;
						F.m_SavedModifTime = ModifTime;
						m_TokenListFiles.push_back(F),
						I.m_pListFile =  &m_TokenListFiles.back();
					};
			}
			assert (I.m_pListFile != NULL);
			CTokenListFile& F = 	*I.m_pListFile;
			F.m_PossibleLemmas.clear();
			FILE* fp = fopen(FileName.c_str(), "r");
			assert(fp);

			if (!fp) 
			{
				ErrorMessage(Format("cannot open %s\n",FileName.c_str()));
				//fclose(tokens);
				return false;
			};
			
			char buffer[1000];
			while (fgets(buffer, 1000, fp))
			{
				string s = buffer;
				Trim(s);
				if (s.empty()) continue;
				RmlMakeUpper(s, m_Language);
				F.m_PossibleLemmas.insert(s);
			};

			fclose(fp);
		};

	};
	//fclose(tokens);
	return true;
};



bool CWorkGrammar::HasOnlyTerminalOnTheRight(CWRI it) const
{
	for (size_t i=0; i<it->m_RightPart.m_Items.size(); i++)
	{
		const CGrammarItem&  I = m_UniqueGrammarItems[ it->m_RightPart.m_Items[i]];
		if (I.m_bMeta) return false;
	};
	return true;
};


size_t CWorkGrammar::GetCountOfSymbolOnTheRight(CWRI it, size_t ItemNo) const
{
	size_t Result = 0;
	for (size_t l=0; l<it->m_RightPart.m_Items.size(); l++)
		if (it->m_RightPart.m_Items[l] == ItemNo)
			Result++;

	return Result;
};

size_t CWorkGrammar::GetCountOfRoots()  const
{
	size_t Result  = 0;
	string Dump;
	for (size_t i=0; i<m_UniqueGrammarItems.size(); i++)
		if (m_UniqueGrammarItems[i].m_bGrammarRoot)
		{	
			Result++;
			Dump += m_UniqueGrammarItems[i].GetDumpString();
			Dump += ",";
		}
	fprintf (stderr, "Roots : %s\n",Dump.c_str());
	
	return Result;
};

int CWorkGrammar::GetFirstRoot()  const
{
	for (size_t i=0; i<m_UniqueGrammarItems.size(); i++)
		if (m_UniqueGrammarItems[i].m_bGrammarRoot)
			return i;
	return -1;
};




size_t	CWorkGrammar::GetRulesCount() const
{
	return  m_EncodedRules.size();
}; 

void  CWorkGrammar::CreateAutomatSymbolInformation() 
{
	m_AutomatSymbolInformation.clear();

	for (TerminalSymbolType i=0; i<m_UniqueGrammarItems.size(); i++)
		m_AutomatSymbolInformation[i] = m_UniqueGrammarItems[i].GetDumpString();
	
};


void	PrintGroups(const CWorkGrammar& Grammar) 
{
	for (CWRI it = Grammar.m_EncodedRules.begin(); it!= Grammar.m_EncodedRules.end(); it++)
	{		
		const CWorkRule& R = *it;;
		printf ("%s ->",Grammar.m_UniqueGrammarItems[R.m_LeftPart].GetDumpString().c_str());
		
		printf ("\n");
	};

};
void	CWorkGrammar::ConvertToPrecompiled(const set<CWorkRule>& EncodedRules) 
{
	fprintf (stderr,"ConvertToPrecompiled\n");
	m_PrecompiledEncodedRules.clear();
	m_PrecompiledEncodedRules.reserve( EncodedRules.size() );
	for (CWRI it = EncodedRules.begin(); it!= EncodedRules.end(); it++)
	{
		CPrecompiledWorkRule R;
		R.m_LeftPart = it->m_LeftPart;
		R.m_SynMainItemNo =  it->m_RightPart.m_SynMainItemNo;
		m_PrecompiledEncodedRules.push_back(R);
	};
	
};

void  CWorkGrammar::BuildAutomat(set<CWorkRule>& EncodedRules) 
{
	CreateAutomatSymbolInformation();

/*	for (CWRI it = EncodedRules.begin(); it!= EncodedRules.end(); it++)
	{
		printf("%s\n", GetRuleStr(*it, -1, false).c_str());
	};*/

	m_TrieHolder.Create(EncodedRules, &m_AutomatSymbolInformation);
}

bool	CWorkGrammar::IsValid() const
{
	for (CWRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
	{
		const CWorkRightRulePart& R = it->m_RightPart;
		if (R.m_SynMainItemNo >= R.m_Items.size())
		{

			ErrorMessage(Format ("%s has no syntax root",GetRuleStr(it).c_str()));
			return false;
		};
	};

	return true;
};

void  UpdateOld2NewMap(map<int, int>& Old2New, size_t& Index, int& MaxNewIndex)
{
	map<int, int>::const_iterator it =  Old2New.find(Index);
	if (it == Old2New.end())
	{
		Old2New[Index] = MaxNewIndex;
		Index = MaxNewIndex;
		MaxNewIndex++;
	}
	else
	{
		Index = it->second;
	};
};

void	CWorkGrammar::DeleteUnusedSymbols()
{
	set<CWorkRule>						NewEncodedRules;
	map<int, int>  Old2New;
	int CountOfUsedSymbols = 0;
	for (WRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
	{
		CWorkRule R = (*it);
		UpdateOld2NewMap(Old2New, R.m_LeftPart, CountOfUsedSymbols);
		for (int l=0; l<R.m_RightPart.m_Items.size(); l++)
			UpdateOld2NewMap(Old2New, (size_t&)R.m_RightPart.m_Items[l], CountOfUsedSymbols);

		NewEncodedRules.insert(R);
	};

	m_EncodedRules = NewEncodedRules;


	vector<CGrammarItem> NewUniqueRuleItems(CountOfUsedSymbols);
	map<int, int>::const_iterator it1 =  Old2New.begin();
	for (; it1 !=  Old2New.end(); it1++)
		NewUniqueRuleItems[it1->second] = m_UniqueGrammarItems[it1->first];

	m_UniqueGrammarItems = NewUniqueRuleItems;
};


/*
A work attribute is an attribute which is used to compare a node with the input text.
For example, attribute "grm" (grammems) contains morphological grammems which should 
be found int the word's properties.
A work attribute is always transferred to a special CGrammarItem's property while a simple attribute
is stored in CGrammarItem::m_Attributes.
A work attribute can be assigned only to nodes which cover one word, not a sequence of words.
For example it is not possible:
[A] -> [B],[C]
D->[A grm="gen"]

Procedure CParseGrammar::CreateNodesForNodesWithWorkAttributes finds for each node [X w],
where "w" is a set of  work attributes, and for each rule [X]->z, 
where X is the same node as "[X w]",  but without work attributes, and adds a rule [X w]->z.
For example,
[D] -> [FILE name="test.txt"]			[D] -> [FILE name="test.txt"]
[B] -> [D]						===>	[B] -> [D]
[A] -> [B grm="gen"]					[A] -> [B grm="gen"]
										[B grm="gen"] -> [D grm="gen"]
										[D grm="gen"] -> [FILE name="test.txt" grm="gen"]


*/


bool SolveNodeWithWorkAttributes(CWorkGrammar& Grammar,  const CGrammarItem& Item, string& ErrorStr)
{
	vector<CWorkRule> NewRules;
	
	for (WRI it = Grammar.m_EncodedRules.begin(); it!= Grammar.m_EncodedRules.end(); it++)
	{
		const CWorkRule& Rule = (*it);
		if (!Grammar.m_UniqueGrammarItems[Rule.m_LeftPart].RuleItemPartialEqual(Item) ) continue;

		//  "Item"  has working attributes, that means that it is used
		//  in the right-hand part, so Grammar.m_UniqueGrammarItems[Rule.m_LeftPart] is not root
		//  since the names of "Item" and  Grammar.m_UniqueGrammarItems[Rule.m_LeftPart] are equal
		Grammar.m_UniqueGrammarItems[Rule.m_LeftPart].m_bGrammarRoot = false;

		CGrammarItem LeftPart = Grammar.m_UniqueGrammarItems[Rule.m_LeftPart];
		LeftPart.CopyNonEmptyWorkAttributesFrom(Item);

		CWorkRule NewRule = Rule;
		NewRule.m_LeftPart = Grammar.GetItemId(LeftPart);
		NewRule.m_RuleId  = Grammar.m_EncodedRules.size() + NewRules.size() + 1;
		
		

		{ // copy working attributes from Item to the root of Rule
			assert (NewRule.m_RightPart.m_SynMainItemNo < NewRule.m_RightPart.m_Items.size() );
			int& MainItemId = NewRule.m_RightPart.m_Items[NewRule.m_RightPart.m_SynMainItemNo];
			CGrammarItem RightPart = Grammar.m_UniqueGrammarItems[MainItemId];
			RightPart.CopyNonEmptyWorkAttributesFrom(Item);
			MainItemId = Grammar.GetItemId(RightPart);
		};

		NewRules.push_back(NewRule);

	};

	Grammar.m_EncodedRules.insert(NewRules.begin(), NewRules.end());

	return true;
};


bool CWorkGrammar::CreateNodesForNodesWithWorkAttributes(string& ErrorStr) 
{

	for (size_t i=0; i < m_UniqueGrammarItems.size(); i++)
	{
		CGrammarItem I = m_UniqueGrammarItems[i];

		if (!I.HasAnyOfWorkingAttributes() ||  !I.m_bMeta)
			continue;

		if (!SolveNodeWithWorkAttributes(*this, I, ErrorStr))
			return false;
	};


	return true;

};


bool CWorkGrammar::CheckCoherence() const
{
	set<size_t> AllLeftParts;
	size_t i =0;

	for (CWRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
	{
		AllLeftParts.insert(it->m_LeftPart);
	};

	for (CWRI it = m_EncodedRules.begin(); it!= m_EncodedRules.end(); it++)
	{
		const CWorkRule& R = (*it);
		for (size_t j =0; j<R.m_RightPart.m_Items.size(); j++)
		{
			size_t id = R.m_RightPart.m_Items[j];
			const CGrammarItem& I = m_UniqueGrammarItems[id];
			if (I.m_bMeta)
			{
				if	(AllLeftParts.find(id) == AllLeftParts.end() )
				{
					ErrorMessage(Format("\"%s\" is used , but not defined\n", I.GetDumpString().c_str()));
					return  false;
				};
			};
		};
	};

	return true;
};



inline size_t get_size_in_bytes (const CPrecompiledWorkRule& t)
{
	return		get_size_in_bytes(t.m_LeftPart) 
			+	get_size_in_bytes(t.m_MetaGroupIndexNo) 
			+	get_size_in_bytes(t.m_SynMainItemNo);
};

inline size_t save_to_bytes(const CPrecompiledWorkRule& t, BYTE* buf)
{
	buf += save_to_bytes(t.m_LeftPart, buf);
	buf += save_to_bytes(t.m_MetaGroupIndexNo, buf);
	buf += save_to_bytes(t.m_SynMainItemNo, buf);
	return get_size_in_bytes(t);
};

inline size_t restore_from_bytes(CPrecompiledWorkRule& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.m_LeftPart, buf);
	buf += restore_from_bytes(t.m_MetaGroupIndexNo, buf);
	buf += restore_from_bytes(t.m_SynMainItemNo, buf);
	return get_size_in_bytes(t);
};


bool	CWorkGrammar::SavePrecompiled(string GrammarFileName) const
{
	
	string PrecompiledFile = MakeFName(GrammarFileName,"grammar_precompiled");
	fprintf (stderr, "SavePrecompiled to %s\n",PrecompiledFile.c_str());

	FILE * fp = fopen(PrecompiledFile.c_str(), "wb");
	if (!fp)
	{
		fprintf (stderr, "Cannot write %s\n",PrecompiledFile.c_str());
		return false;
	};
	fprintf(fp, "%i\n", (int)m_Language);
	fprintf(fp, "%i\n", m_UniqueGrammarItems.size());

	for (size_t SymbolNo=0; SymbolNo<m_UniqueGrammarItems.size(); SymbolNo++)
	{
		{
			// checking
			string q = m_UniqueGrammarItems[SymbolNo].toString().c_str();
			CGrammarItem I;
			bool b = I.fromString(q);
			assert (b);
			assert (I == m_UniqueGrammarItems[SymbolNo]);
		}	
		fprintf (fp, "%s", m_UniqueGrammarItems[SymbolNo].toString().c_str());
	};

	fprintf(fp, "%i\n", m_PrecompiledEncodedRules.size());
	WriteVectorInner(fp, m_PrecompiledEncodedRules);

	fclose(fp);

	//assert (!m_AutomatSymbolInformation.empty());
	m_TrieHolder.Save(GrammarFileName);
	return true;
};



bool	CWorkGrammar::LoadFromPrecompiled()
{
	time_t t1;
	time(&t1) ;
	string PrecompiledFile = MakeFName(m_SourceGrammarFile,"grammar_precompiled");
	fprintf (stderr, "loading from precompiled grammar %s\n", PrecompiledFile.c_str());
	FILE * fp = fopen(PrecompiledFile.c_str(), "rb");
	if (!fp)
	{
		ErrorMessage (Format("Cannot open %s", PrecompiledFile.c_str()));
		return false;

	};

	int Lang;
	fscanf(fp, "%i\n", &Lang);
	m_Language = (MorphLanguageEnum)Lang;

	int Count;
	fscanf(fp, "%i\n", &Count);

	m_UniqueGrammarItems.clear();
	for (size_t i=0; i < Count; i++)
	{
		string OneRecord;
		for (size_t LineNo=0; LineNo < 7; LineNo++)
		{
			char buffer[1024];
			if (!fgets(buffer, 1024, fp))
			{
				ErrorMessage (Format("Cannot read %s", PrecompiledFile.c_str()));
				return false;
			};
			OneRecord += buffer;
		};

		CGrammarItem I;
		bool b = I.fromString(OneRecord);
		if (!b)
		{
			ErrorMessage (Format("Cannot read %s in %s", PrecompiledFile.c_str(), OneRecord.c_str() ));
			return false;
		};
		m_UniqueGrammarItems.push_back(I);
		
	};

	{
		char buffer[256];
		if (!fgets(buffer, 256, fp)) return false;
		Count = atoi(buffer);
	};
	m_PrecompiledEncodedRules.clear();
	ReadVectorInner(fp, m_PrecompiledEncodedRules, Count);

	fclose(fp);

	CreateAutomatSymbolInformation();

	assert (!m_AutomatSymbolInformation.empty());
	if (!m_TrieHolder.Load(&m_AutomatSymbolInformation, m_SourceGrammarFile))
	{
		ErrorMessage ("Cannot load automat");
		return false;
	};

	time_t t2;
	time(&t2) ;

	fprintf (stderr, "Seconds = %i\n",  t2-t1);


	return true;
};

// adding a new root non-terminal "NewRoot" and a special 
// symbol for end of input ("$")
bool CWorkGrammar::AugmentGrammar()
{
	size_t Count = GetCountOfRoots();
	
	if (Count != 1)
	{
		ErrorMessage("A simple grammar should have only one root!" );
		assert (Count == 1);
		return false;
	};
	size_t i=0;
	for (; i<m_UniqueGrammarItems.size(); i++)
		if (		(m_UniqueGrammarItems[i].m_ItemStrId == "NewRoot")
				||	(m_UniqueGrammarItems[i].m_ItemStrId == "$")
			)
		{
			assert (false);
			return false;
		};

	
	for (i=0; i<m_UniqueGrammarItems.size(); i++)
		if (m_UniqueGrammarItems[i].m_bGrammarRoot)
		{
			break;
		};
	
	CGrammarItem I;

	
	//  adding a special symbol (end of input)
	I.m_ItemStrId = "$";
	I.m_Token = "$";
	I.m_bMeta = false;
	// w
	m_UniqueGrammarItems.push_back(I);

	//  adding a new root
	I.m_ItemStrId = "NewRoot";
	I.m_bMeta = true;
	m_UniqueGrammarItems.push_back(I);


	CWorkRule R;
	R.m_RuleId = 0;
	R.m_RuleFeaturesNo = m_RuleFeatures.size();
	m_RuleFeatures.push_back(vector<CRuleFeature>());
	R.m_LeftPart =  m_UniqueGrammarItems.size() - 1;
	R.m_RightPart.m_SynMainItemNo = 0;
	R.m_RightPart.m_Items.push_back(i);
	R.m_RightPart.m_Items.push_back(m_UniqueGrammarItems.size()-2);
	m_EncodedRules.insert(R);
	return true;
};

size_t	CWorkGrammar::GetNewRoot() const
{
	assert (!m_UniqueGrammarItems.empty());
	assert (m_UniqueGrammarItems.back().m_ItemStrId == "NewRoot");
	return m_UniqueGrammarItems.size() - 1;
};

size_t	CWorkGrammar::GetEndOfStreamSymbol() const
{
	assert (m_UniqueGrammarItems.size()  > 1);
	assert (m_UniqueGrammarItems.back().m_ItemStrId == "NewRoot");
	assert (m_UniqueGrammarItems[m_UniqueGrammarItems.size() - 2].m_ItemStrId == "$");
	return m_UniqueGrammarItems.size() - 2;

};




	

bool LoadGrammarForGLR(CWorkGrammar& WorkGrammar, bool bUsePrecompiledAutomat, bool bPrintRules)
{
	if (!BuildWorkGrammar(WorkGrammar, bPrintRules))
	{
		return false;
	};

	if (!WorkGrammar.LoadOptions())
		return false;

	if (bUsePrecompiledAutomat)
	{
		
		if (!WorkGrammar.AugmentGrammar())
		{
			return false;
		};
		size_t  SaveSize = WorkGrammar.m_UniqueGrammarItems.size();
		if (!WorkGrammar.LoadFromPrecompiled()) return false;
		if (SaveSize != WorkGrammar.m_UniqueGrammarItems.size())
		{
			fprintf (stderr, "Number of Symbols in the precompiled  version = %i\n", WorkGrammar.m_UniqueGrammarItems.size());
			fprintf (stderr, "Number of Symbols in the current version = %i\n", SaveSize);
			assert (false);
			//  should  be the same size, the difference means that the precompiled version 
			//  contain a different grammar
			return false;
		};

		if (!WorkGrammar.m_GLRTable.LoadGLRTable(MakeFName(WorkGrammar.m_SourceGrammarFile,"table")))
		{
			ErrorMessage ("Cannot load GLR Table");
			return false;
		};

	}
	else
	{
		if (WorkGrammar.m_bEnableRootPrefix)
		{
			size_t  PrefixSize = 3;
			fprintf (stderr, "build RootPrefixes(%i) \n", PrefixSize);
			WorkGrammar.BuildRootPrefixes(PrefixSize);
			fprintf (stderr, "build automat for prefixes \n");
			WorkGrammar.BuildAutomat(WorkGrammar.m_RootPrefixes);
			WorkGrammar.ConvertToPrecompiled(WorkGrammar.m_RootPrefixes);
		};

		if (!WorkGrammar.AugmentGrammar())
		{
			return false;
		};

		WorkGrammar.Build_FIRST_Set();
		WorkGrammar.Build_FOLLOW_Set();
	

		if (!WorkGrammar.m_GLRTable.BuildGLRTable())
		{
			return false;
		};


	}

	
	//WorkGrammar.Print();

	/*FILE * fp = fopen ("table","w");
	WorkGrammar.m_GLRTable.PrintGLRTable(fp);
	fclose(fp);*/

	
	{
		fprintf (stderr, "create token list for each node\n");
		string ErrorStr;
		time_t t1,t2;
		time(&t1) ;

		if (!WorkGrammar.CreateTokenList(ErrorStr))
		{
			ErrorMessage(ErrorStr);
			return false;
		};
		time(&t2);fprintf (stderr, "Seconds = %i\n",  t2-t1);
		fprintf (stderr, "ok\n");
	};

	return true;

};
