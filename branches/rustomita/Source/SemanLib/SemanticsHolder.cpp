#include "stdafx.h"
#include "SemanticsHolder.h"






// ==============================   класс CSemPattern ==========================
  
bool CCollocItem::InitCollocItem (string S) 
{
      Item = S;
	  Trim(Item);
	  assert  (Item != "");
	  RelOperator1 = "";
	  RelOperator2 = "";

	  long i = Item.find('[');

	  if (i==0)
	  {
		  return false;
	  };

	  if (i != -1)
	  {
          string Q = Item.substr(i+1);
		  long k = Q.find('[');
		  if (k != -1)
		  {
			  RelOperator2 = Q.substr(k+1);
			  RelOperator2.erase(RelOperator2.length()-1,1);
			  Q.erase (k);
		  };
		  RelOperator1 = Q.substr(0, Q.length()-1);

		  Item.erase (i, Item.length() - i);
	  };
	  return true;
};

bool CCollocItem::IsHole() const  
{ 
	return (Item[0] == '_');
};






// ==============================   класс CSemanticsHolder ==========================


CSemanticsHolder::CSemanticsHolder() 
{
	m_LastIndicesUpdateTime = 0;
	m_bDontLoadExamples = false;
};

const vector<CAbstractArticle>* CSemanticsHolder::GetAbstractArticles(DictTypeEnum type ) const
{
	AbsractArticleMap::const_iterator it = m_AbstractArticles.find(type);
	if (it == m_AbstractArticles.end())
		return 0;
	else
		return &(it->second);
};

bool CSemanticsHolder::ReadAbstractArticles(DictTypeEnum type)
{
	vector<CAbstractArticle> Articles;
	long UnitNo = GetRoss(type)->UnitsLowerBound("+");
	long UnitsCount = GetRoss(type)->GetUnitsSize();
	while  (    (UnitNo < UnitsCount) 
		  &&  (UnitNo != ErrUnitNo)
		)
	{
	  CAbstractArticle A;
	  A.m_UnitStr = GetRoss(type)->GetEntryStr(UnitNo);
  
	  if ( (unsigned char)A.m_UnitStr[0] !=  '+') break;

	  if (!GetRoss(type)->IsEmptyArticle(UnitNo))
	  {
		 for (size_t i = GetRoss(type)->GetUnitStartPos(UnitNo); i<= GetRoss(type)->GetUnitEndPos(UnitNo); i++)
		 {
			TCortege C = GetCortege(GetRoss(type), i);
			string FieldStr = (const char*)GetRoss(type)->Fields[C.m_FieldNo].FieldStr;
			if  ( FieldStr == "TYP" ) 
			{
				string S = WriteToString(GetRoss(type), (char*)(GetRoss(type)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
				Trim(S);
				if (S == "ДОБАВЛЕНИЕ")
					A.m_Type = atAdditionArticle;
				else
				   if (S == "ЗАГЛУШКА")
					 A.m_Type = atArticlePlug;
				   else
					   A.m_Type = atEmptyType;

			}

			if  ( FieldStr == "VAL" ) 
  			  A.m_Vals.push_back(CValency (C, GetRossHolder(type)->MainWordVarNo, GetRossHolder(type)));
			
			if (C.IsEqual())
			{
				if  (FieldStr == "GF" ) 
				{
				  CGramInfo I;
				  string GramFet = WriteToString(GetRoss(type), (char*)(GetRoss(type)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
				  GetCustomGrammems(GramFet, I.m_Grammems, I.m_PartOfSpeechMask);
				  A.m_GramInfos.push_back(I);
				};
				
				if  ( FieldStr == "CLAUSE") 
				{
					string S = WriteToString(GetRoss(type), (char*)(GetRoss(type)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
					Trim(S);
					long  Type =  GetRusGramTab()->GetClauseTypeByName(S.c_str());
					A.m_ClauseTypes.push_back (Type);
				};

				if  ( FieldStr == "CAT" ) 
				{
					string S = WriteToString(GetRoss(type), (char*)(GetRoss(type)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
					Trim(S);
					A.m_SemTypes.push_back(S);
				};
				if  ( FieldStr == "SF" ) 
				{
					string S = WriteToString(GetRoss(type), (char*)(GetRoss(type)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
					Trim(S);
					A.m_SemFets.push_back(S);
				};

				if  ( FieldStr == "LEX" ) 
				{
					string S = WriteToString(GetRoss(type), (char*)(GetRoss(type)->Fields[C.m_FieldNo].m_Signats[C.GetSignatNo()].sFrmt), C);
					Trim(S);
					A.m_LexFets.push_back(S);
				};

           

			}
			else
			 A.m_Article.push_back (C);
		 };
		 A.m_UnitNo = UnitNo;
		 Articles.push_back(A);

	  };
	  UnitNo++;


	};

	m_AbstractArticles[type] = Articles;
	return  true;
};



void CSemanticsHolder::GetCustomGrammems (string GramFet, QWORD& Grammems, DWORD& Pose)
{
	assert(GetRusGramTab() != 0);
	Trim(GramFet);
	QWORD G;
	BYTE Pos;
	if (GetRusGramTab()->ProcessPOSAndGrammemsIfCan(GramFet.c_str(), &Pos, &G))
	{
		Grammems = G;
		if (Pos == VERB)
			Pose  = (1<<VERB) | (1<<INFINITIVE) | (1 << ADVERB_PARTICIPLE) | (1 << PARTICIPLE) | (1 << PARTICIPLE_SHORT);
		else
			Pose  = (1<<(size_t)Pos);
	}
	else
	{
		GramFet = "С "+ GramFet;
		if (GetRusGramTab()->ProcessPOSAndGrammemsIfCan(GramFet.c_str(), &Pos, &G) )
		{
			Grammems = G;
			Pose  = 0xffffffff;
		}
		else
		{
			Grammems = 0xffffffff;
			Pose  = 0xffffffff;
		};

	};
};

string CSemanticsHolder::GetContentsOborStr(long UnitNo, vector<CUnitContent> & Contents)
{
	CUnitContent T;
	T.m_UnitNo = UnitNo;
	vector<CUnitContent>::iterator It = lower_bound(Contents.begin(),Contents.end(), T);
	if (    
			(It < Contents.end())
			&& (*It == T)
		)
		return It->m_UnitStr;
	return "";
};	






bool GetLemmaFromTitle (string S, long PlaceNo, string& Lemma)
{
	EngRusMakeUpper(S);
	for (size_t i=0; i< PlaceNo + 1; i++)
	{
		int u = S.find_first_of(" ");
		Lemma = S.substr (0, u);
		if (Lemma.empty())  return false;
		S.erase(0,Lemma.length());
		TrimLeft(S);
		
	};
	return true;
};



void InitThesList (const CThesaurus* Thes, string ConceptStr,	StringVector& Vec)
{
	Vec.clear();
	vector<int> LowerTermins;
	Thes->QueryLowerTermins(ConceptStr.c_str(), morphRussian, LowerTermins);
	long Count = LowerTermins.size();
	for (long i=0; i <Count; i++)
	{
			const CInnerTermin& T = Thes->m_Termins[LowerTermins[i]];
			string TerminStr =  T.m_TerminStr;
			EngRusMakeUpper(TerminStr);
			Vec.push_back(TerminStr);
	};
	sort(Vec.begin(),Vec.end());
};

bool CSemanticsHolder::InitTimeThesLists()
{
	InitThesList(GetThes(OmniThes), "MONTH", m_RusMonths);
	InitThesList(GetThes(OmniThes), "DAY-OF-WEEK", m_RusWeekDays);	
	return true;
};

// преобразование словаря групп времени в удобный для поиска вид
bool CSemanticsHolder::InitTimeUnits()
{
 // инициализация констант 
 BYTE GramFunctDomNo = GetRoss(TimeRoss)->GetDomenNoByDomStr("D_GRAM_FUNCT");
 int AbbrFunctName = GetRoss(TimeRoss)->GetItemNoByItemStr("СОКР", GramFunctDomNo);
 int AbbrFunctPluralName = GetRoss(TimeRoss)->GetItemNoByItemStr("СОКР_мн", GramFunctDomNo);
 m_TimeAbbrPairs.clear();
 m_TimeUnits.clear();
 
 // идем по всем статьям словаря групп времени 
 for (size_t UnitNo =0; UnitNo < GetRoss(TimeRoss)->GetUnitsSize(); UnitNo++)
 {
   try {
    
    CTimeUnit U;
    U.m_UnitNo = UnitNo;
	
	if (!GetRoss(TimeRoss)->IsEmptyArticle(UnitNo))
		// по словарной статье 
	for (size_t i = GetRoss(TimeRoss)->GetUnitStartPos(UnitNo); i<= GetRoss(TimeRoss)->GetUnitEndPos(UnitNo); i++)
	{
	  TCortege C = GetCortege(GetRoss(TimeRoss), i);
	  //незаполненное поле?
	  if (C.m_DomItemNos[0] == -1) continue;
	  // строю массив U.m_Places по полю CONTENT
	  string FieldStr = (const char*)GetRoss(TimeRoss)->Fields[C.m_FieldNo].FieldStr;
      if (    (FieldStr == "CONTENT") 
	       && (C.m_LeafId == 0) 
		   && (C.m_BracketLeafId == 0) 
		 )
	  {
		 string Lemma;
		 string Contents = GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[0]);
 	     for (BYTE PlaceNo =0; GetLemmaFromTitle(Contents, PlaceNo, Lemma);  PlaceNo++)
	       U.m_Places.push_back(Lemma);
	  };
	 
      if (    (FieldStr == "RESTR") 
	       && (C.m_LeafId == 0) 
		   && (C.m_BracketLeafId == 0) 
		 )
	  {
		 string Contents = GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[0]);
		 if (Contents == "свобод")
			 U.m_bCanFillNotTimeValency = true;
	  };

	  // инициализирую перечень всех необходимых синтаксических отношений их поля SYNREP
	  if (    (FieldStr == "SYNREP") 
	       && (C.m_LeafId == 0) 
		   && (C.m_BracketLeafId == 0) 
		 )
	  {
		  long PlaceNo1 = atoi(GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[1]));
		  long PlaceNo2 = atoi(GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[2]));
		  if (!PlaceNo1 || !PlaceNo2) continue;
		  string SynGrp = GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[0]);
		  //rml_TRACE  (SynGrp);
	      U.m_Rels.push_back(CSynRelation(PlaceNo1-1, PlaceNo2-1, SynGrp));
	  };

	  // инициализирую глобальный перечень наборов (полное временное слово, аббревиатура, аббревиатурная функция),
	  // который называется m_TimeAbbrPairs
	  // аббревиатурная функция = АББР_мн, АББР, АББР_ед
	  // например:(год, гг.,АББР_мн)
	  //          (год, г., АББР_ед)
  	  if (    (FieldStr == "DERIV") 
	       && (C.m_LeafId == 0) 
		   && (C.m_BracketLeafId == 0) 
		 )
	  {
		  if (    (C.m_DomItemNos[0] != AbbrFunctPluralName)
			   && (C.m_DomItemNos[0] != AbbrFunctName)
			 ) 
		  continue;
		  string FullForm = GetRoss(TimeRoss)->GetEntryStr(UnitNo);
		  EngRusMakeUpper(FullForm);
		  string AbbrForm = GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[1]);
		  EngRusMakeUpper(AbbrForm);
		  m_TimeAbbrPairs.push_back(CAbbrFunct(AbbrForm, FullForm, GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[0])));
	  };

	   // инициализирую перечень лексического заполнения дырок, который берется из 
	   // полей LEX и PREP

	   if   (FieldStr == "LEX")
		 {
		    string S =   GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[0]);
			EngRusMakeUpper(S);
			long LexFillingNoNo = U.GetLexicalFillingNo(CTimeLexicalFilling(C.m_LeafId, C.m_BracketLeafId));
			U.m_LexicalFillings[LexFillingNoNo].m_LexFets.push_back(stringLong(S,C.m_LevelId));
		 }
		 else
		 if (FieldStr == "PREP")
		 {
             string Prep = GetRossHolder(TimeRoss)->GetDomItemStrInner(C.m_DomItemNos[0]);
		     WORD PrepNo = GetRossHolder(OborRoss)->LocateUnit(Prep.c_str(),1);
             if (PrepNo == ErrUnitNo) 
			 {
				 string Q =Format ("Предлог %s в статье %s не найден в словаре оборотов", Prep.c_str(), GetRoss(TimeRoss)->GetEntryStr(UnitNo).c_str());
				 ErrorMessage (Q);
				 continue;
			 };
			 long LexFillingNoNo = U.GetLexicalFillingNo(CTimeLexicalFilling(C.m_LeafId, C.m_BracketLeafId));
			 U.m_LexicalFillings[LexFillingNoNo].m_Preps.push_back(LongLong(PrepNo,C.m_LevelId));
		 };

	};

	
	m_TimeUnits.push_back(U);
   }
	catch (...) 
	{
		ErrorMessage (Format("Cannot index article \"%s\"", GetRoss(TimeRoss)->GetEntryStr(UnitNo).c_str()));
		return false;
	};
   };
 
	return true;
};

bool CSemanticsHolder::PrintRusEquivs(string strFileName, DictTypeEnum type)
{
	/*ofstream file;
	file.open(strFileName.c_str());
	vector<CEngUnitNoToRusUnit>& equivs = GetRusEquivIndexes(type);
	for(int i = 0 ; i < equivs.size() ; i++ )
	{
		string str = "<" + equivs[i].m_RusUnitStr + ">";
		string ss = Format("%d", equivs[i].m_RusMeanNum);
		str = str + " " + ss;
		ss = GetRoss(type)->GetEntryStr(equivs[i].m_EngUnitNo);
		str = ss + " :" + str;
		file << str.c_str() << endl;
	}

	file.close();*/
	return true;
}

bool CSemanticsHolder::CreateEngOborIndex()
{
	return CreateEngDictIndex(EngObor, m_RusEquivsEngObor);
}

bool CSemanticsHolder::CreateAROSSIndex()
{
	return CreateEngDictIndex(Aoss, m_RusEquivs);
}

bool CSemanticsHolder::CreateEngCollocsROSSIndex()
{
	return CreateEngDictIndex(EngCollocRoss, m_RusEquivsEngColsocs);
}



// создает индекс RusEquivs по словарю type
bool CSemanticsHolder::CreateEngDictIndex(DictTypeEnum type, vector<CEngUnitNoToRusUnit>& RusEquivs)
{
	RusEquivs.clear();
	long UnitsCount = GetRoss(type)->GetUnitsSize();
	BYTE RusFieldNo = GetRoss(type)->GetFieldNoByFieldStr("RUS");
	if (RusFieldNo == ErrUChar) return false;
	for(long i = 0 ; i <  UnitsCount ; i++ )
	{
		long EndPos = GetRoss(type)->GetUnitEndPos(i);
		for(long j = GetRoss(type)->GetUnitStartPos(i) ; j <=  EndPos ; j++ )
			if( RusFieldNo == GetRoss(type)->GetCortegeFieldNo(j) )
			{
				CEngUnitNoToRusUnit Item;
				TCortege C = GetCortege (GetRoss(type), j);
				if( C.m_DomItemNos[0] == -1 )
					continue;

				Item.m_RusUnitStr = GetRossHolder(type)->GetDomItemStrInner(C.m_DomItemNos[0]);

				if(C.m_DomItemNos[1] != -1)
				{					
					string buf = GetRossHolder(type)->GetDomItemStrInner(C.m_DomItemNos[1]);
					assert(buf.length() == 1);
					Item.m_RusMeanNum = buf[0] - '0';
					
				}
				Item.m_EngUnitNo = i;
				RusEquivs.push_back(Item);				
			}
	}	
	sort(RusEquivs.begin(), RusEquivs.end());

	return true;
}

vector<CEngUnitNoToRusUnit>& CSemanticsHolder::GetRusEquivIndexes(DictTypeEnum type)
{
	assert(( type == Aoss ) || ( type == EngCollocRoss ) || (type == EngObor) );
	switch( type)
	{
	case Aoss:
			  {
				return m_RusEquivs;
			  }
	case EngCollocRoss:
			  {
				return m_RusEquivsEngColsocs;
			  }
	case EngObor:
			  {
				return m_RusEquivsEngObor;
			  }
	}

	return m_RusEquivs;
}


bool CSemanticsHolder::CreateLexFunIndexes(const CDictionary* pRoss, vector<SLexFunIndexes>& LexFunIndexesV)
{
	LexFunIndexesV.clear();
	long UnitsCount = pRoss->GetUnitsSize();
	long LFDomNo = pRoss->GetDomenNoByDomStr("D_LF");
	long DigDomNo = pRoss->GetDomenNoByDomStr("D_NUMBERS");
	long SXFieldNo = pRoss->GetFieldNoByFieldStr("SF");
	long LFFieldNo = pRoss->GetFieldNoByFieldStr("LF");

	for(long j = 0 ; j <  UnitsCount ; j++ )
	{
		int iEndPos = pRoss->GetUnitEndPos(j);
		for(int i = pRoss->GetUnitStartPos(j) ; i <=  iEndPos ; i++ )
		{
			if( SXFieldNo == pRoss->GetCortegeFieldNo(i) )
			{
				long itemLexFunValue;
				long itemLF = pRoss->GetCortegeItem(i,0);
				if( pRoss->GetDomItemDomNo(itemLF) ==  LFDomNo )
				{
					itemLexFunValue = pRoss->GetCortegeItem(i,1);
					if(itemLexFunValue != -1)
					{
						string strRusVerb = (const char*)pRoss->GetDomItemStr(itemLexFunValue);							
						SLexFunIndexes LexFunIndex;
						LexFunIndex.m_LexFun = (const char*)pRoss->GetDomItemStr(itemLF);
						LexFunIndex.m_LexFunArg.m_UnitStr = strRusVerb;
						long item = pRoss->GetCortegeItem(i,2);
						if( item == DigDomNo )
						{
							LexFunIndex.m_LexFunArg.m_MeanNum = string((const char*)pRoss->GetDomItemStr(item))[0] - '0';
						}
						LexFunIndex.m_LexFunValue.m_UnitStr = pRoss->GetEntryStr(j);
						LexFunIndex.m_LexFunValue.m_MeanNum = pRoss->GetUnitMeanNum(j);
						LexFunIndexesV.push_back(LexFunIndex);
					}
				}
			}
			if( LFFieldNo == pRoss->GetCortegeFieldNo(i) )
			{
				long itemLF = pRoss->GetCortegeItem(i,0);
				if( pRoss->GetDomItemDomNo(itemLF) ==  LFDomNo )
				{					
					long itemLexFunValue = pRoss->GetCortegeItem(i,1);
					if(itemLexFunValue != -1)
					{
						string strRusVerb = (const char*)pRoss->GetDomItemStr(itemLexFunValue);							
						SLexFunIndexes LexFunIndex;
						LexFunIndex.m_LexFun = (const char*)pRoss->GetDomItemStr(itemLF);
						LexFunIndex.m_LexFunValue.m_UnitStr = strRusVerb;
						long item = pRoss->GetCortegeItem(i,2);
						if( item == DigDomNo )
						{
							LexFunIndex.m_LexFunValue.m_MeanNum = string((const char*)pRoss->GetDomItemStr(item))[0] - '0';
						}
						LexFunIndex.m_LexFunArg.m_UnitStr = pRoss->GetEntryStr(j);
						LexFunIndex.m_LexFunArg.m_MeanNum = pRoss->GetUnitMeanNum(j);
						LexFunIndexesV.push_back(LexFunIndex);
					}
				}				
			}

		}
	}

	return true;
}


bool WordInList(const char* word_list, int count, string word)
{
	for(int i = 0 ; i < count ; i++ )
		if( !strcmp(word.c_str() , (word_list + (i * MaxWordLen)))  )
			return true;

	return false;
}


bool WordMeanComp(const CDictReference& arg1, const CDictReference& arg2)
{
	if( (arg1.m_MeanNum == 10) || (arg2.m_MeanNum == 10) )
		return arg1.m_UnitStr < arg2.m_UnitStr;

	if( arg1.m_UnitStr == arg2.m_UnitStr )
		return arg1.m_MeanNum < arg2.m_MeanNum;

	return arg1.m_UnitStr < arg2.m_UnitStr;
}

bool LexFunArgComp(const SLexFunIndexes& arg1, const SLexFunIndexes& arg2)
{
	return WordMeanComp(arg1.m_LexFunArg, arg2.m_LexFunArg);
}

bool LexFunValueComp(const SLexFunIndexes& arg1, const SLexFunIndexes& arg2)
{
	return WordMeanComp(arg1.m_LexFunValue, arg2.m_LexFunValue);// && (arg1.m_LexFun == arg2.m_LexFun);
}




const char OborotDels[] = " \t";

bool BuildByFieldContents(string s, WORD UnitNo, vector<CUnitContent>& Vect)
{
	size_t i = s.find("(");
	if (i == s.npos) 
	{	
		
		char q[200];
		strcpy (q, s.c_str());
		char* t = q + strspn (q,OborotDels);		
		if (s.find("+") == s.npos)		
		{
			CUnitContent U;
			U.m_UnitStr = t;
			Trim(U.m_UnitStr);
			U.m_UnitNo = UnitNo;
			Vect.push_back(U);
		};
		
	}
	else
	{
		size_t k = s.find (")");
		if (k == s.npos)
		{
			ErrorMessage (Format ("Error in parenthesis  in oborot %s", s.c_str()));
			return false;
		};
		size_t last_j = i;
        for (size_t j =i+1; j <= k; j++)
		{
          if  (    (j == k) 
			    || (s[j] == '|')) 
		  {
             string q;
			 if (i >0) q += s.substr(0,i);
			 q += " ";
			 q += s.substr(last_j+1, j-last_j-1);
			 q += " ";
		     if (k-1 < s.length()) q += s.substr(k+1);
			 if (!BuildByFieldContents(q, UnitNo, Vect)) 
				 return false;
			 last_j = j;
		  };

		};

	};

	return true;
};



bool BuildContensField(const CDictionary* Dict, vector<CUnitContent>& Vect)
{
	Vect.clear();
	long size = Dict->GetUnitsSize();
	string strField = "CONTENT";
	for(long j = 0 ; j < size ; j++ )
	{	
		
		int iEndPos = Dict->GetUnitEndPos(j);
		for(int i = Dict->GetUnitStartPos(j) ; i <=  iEndPos ; i++ )
		{
			long FieldNo = Dict->GetCortegeFieldNo(i);		
			if( Dict->Fields[FieldNo].FieldStr == strField )
			{
				TCortege cortege;
				cortege = GetCortege(Dict, i);
				string s = (const char*)Dict->GetDomItemStr(cortege.m_DomItemNos[0]); 
				if (!BuildByFieldContents(s, j, Vect) )
					return false;
			}
		}
	}
	return true;
};


bool CSemanticsHolder::TokenizeDoubleConj()
{
 vector<CUnitContent> vectorOborStr;

 if (!BuildContensField(GetRoss(OborRoss), vectorOborStr) )
	 return false;

 // идем по всем статьям словаря групп времени 
 for (size_t _UnitNo =0; _UnitNo < vectorOborStr.size(); _UnitNo++)
 {
   try 
   {
		string Contents = vectorOborStr[_UnitNo].m_UnitStr;
		if (Contents.find("...") == -1) continue;
		StringTokenizer token(Contents.c_str(), " ");
		CDoubleConj DoubleConj;
		const char *word;
		bool InFirstPart = true;
		while( word = token() )
		{
			string s = word;
			EngRusMakeUpper(s);
			
			if( !strcmp(word, "..." ) )
				InFirstPart = false;
			else
				if (InFirstPart)
					DoubleConj.m_FirstPart.push_back(s);
				else
					DoubleConj.m_SecondPart.push_back(s);
		}	

		DoubleConj.m_UnitNo = vectorOborStr[_UnitNo].m_UnitNo;
		DoubleConj.m_bRepeating = (DoubleConj.m_FirstPart == DoubleConj.m_SecondPart);
		// проверка на повторы
		long i=0;
		for (; i < m_DisruptConj.size(); i++)
			if  (   (m_DisruptConj[i].m_FirstPart == DoubleConj.m_FirstPart)
				  &&(m_DisruptConj[i].m_SecondPart == DoubleConj.m_SecondPart)
				)
			break;
		if (i == m_DisruptConj.size())
		m_DisruptConj.push_back(DoubleConj);

 	}
	catch (...) 
	{
		ErrorMessage (Format ("Cannot index article \"%s\"", GetRoss(OborRoss)->GetEntryStr(vectorOborStr[_UnitNo].m_UnitNo).c_str()));
		return false;
	};
 }
 return true;
}



bool  CSemanticsHolder::BuildOborottos ()
{
	try { 
		Oborottos.clear();
		size_t count = GetRoss(OborRoss)->GetUnitsSize();
		for (size_t UnitNo=0; UnitNo<count; UnitNo++)
		{
			CObor O;
			O.m_UnitStr =  GetRoss(OborRoss)->GetEntryStr(UnitNo);
			if (GetRossHolder(OborRoss)->HasItem (UnitNo, "GF","ПОДЧ_СОЮЗ","D_PART_OF_SPEECH",0,0))
				O.m_bRusSubConj = true;

			if (GetRossHolder(OborRoss)->HasItem (UnitNo, "RESTR","подл","D_VP_SPECIF",0,0))
				O.m_bRusSubConjCanBeAfterSubject = true;

			if (GetRossHolder(OborRoss)->HasItem (UnitNo, "GF","СОЧ_СОЮЗ","D_PART_OF_SPEECH",0,0))
				O.m_bRusCoordConj = true;

			if	(		GetRossHolder(OborRoss)->HasItem (UnitNo, "GF","ПРЕДЛ","D_PART_OF_SPEECH",0,0) 
					|| GetRossHolder(OborRoss)->HasItem (UnitNo, "GF","ПРОСТ_ПРЕДЛ","D_PART_OF_SPEECH",0,0) 
				)
				O.m_bRusOborPrep = true;

			if (		GetRossHolder(OborRoss)->HasItem (UnitNo, "GF","НАР","D_PART_OF_SPEECH",0,0)
&&	GetRossHolder(OborRoss)->HasItem (UnitNo, "SF","MODL","D_SEM_REL",0,0)				)
				O.m_bRusModalOborAdverbial = true;

			if (	GetRossHolder(OborRoss)->HasItem (UnitNo, "GF","ВВОДН","D_PART_OF_SPEECH",0,0) )
				O.m_bRusIntrExpr = true;

			if ( GetRoss(OborRoss)->IncludeArticle(UnitNo, "GF = * НАР : УСИЛ" ) )
				O.m_bRusNegOborAdverbial = true;

			if ( GetRossHolder(OborRoss)->HasItem (UnitNo, "GF","НАР","D_PART_OF_SPEECH",0,0) )
				O.m_bRusOborAdverbial = true;

			Oborottos.push_back(O);
		};

	}
	catch (...)
	{
		ErrorMessage("Error while reading oborots occured");
		return false;

	};

	return TokenizeDoubleConj();
};

bool CRossHolder::HasBeenModified(long T) 
{
	return m_LastUpdateTime > T;
};


bool CSemanticsHolder::BuildEngOborStr()
{

	return BuildContensField(GetRoss(EngObor), m_vectorEngOborStr);
}

bool CSemanticsHolder::BuildEngCollocsStr()
{	
	return BuildContensField(GetRoss(EngCollocRoss), m_vectorEngCollocStr);
}


void CSemanticsHolder::GetPrepsFromArticle (const CDictionary* Ross, long UnitNo, BYTE LeafId, BYTE BracketLeafId, vector<CRossInterp>& Preps)
{
 BYTE FieldNo = Ross->GetFieldNoByFieldStr("PREP");

 if (!Ross->IsEmptyArticle(UnitNo))
	for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= Ross->GetUnitEndPos(UnitNo); i++)
	  if (     (Ross->GetCortegeFieldNo(i) == FieldNo) 
		    && (Ross->GetCortegeLeafId(i) == LeafId) 
			&& (Ross->GetCortegeBracketLeafId(i) == BracketLeafId) 
		 )
	  {
         string Prep = (const char*)Ross->GetDomItemStr(Ross->GetCortegeItem(i,0));
		 WORD PrepNo = GetRossHolder(OborRoss)->LocateUnit(Prep.c_str(),1);
         if (PrepNo != ErrUnitNo) 
			 Preps.push_back (CDictUnitInterp(OborRoss, PrepNo));
	  };
};





/*
Выдает по парадигме прилагательного наречие, которое совпадает с краткой формой среднего рода
этого прилагательного, например: 
	"красивый" (ПРИЛ) -> "красиво" (НАР)
	"лучше" (ПРИЛ) -> "хорошо" (НАР)
*/
UINT CSemanticsHolder::GetAdverbWith_O_ByAdjective (UINT AdjParadigmId, string AdjWordForm)
{
	string AdvLemma;	

	CFormInfo Paradigm;
	try {
		if (!GetRusLemmatizer()->CreateParadigmFromID(AdjParadigmId, Paradigm))
			return -1;
	}
	catch (...) {
		return -1;
	};


	// ищем краткое прилагательное среднего рода
	long k=0;
	for (; k < Paradigm.GetCount(); k++)
	{
		string Form = Paradigm.GetWordForm(k);
		string AnCode = Paradigm.GetAncode(k);
		QWORD Grammems;
        BYTE POS;
        GetRusGramTab()->ProcessPOSAndGrammems(AnCode.c_str(), POS, Grammems );

        if (( Grammems & _QM(rNeutrum)) && POS==ADJ_SHORT)
					break;
	};
	if (k == Paradigm.GetCount()) return -1;

	AdvLemma = Paradigm.GetWordForm(k);

	return GetFirstParadigmId(morphRussian, AdvLemma,  (1<<ADV));
};






// ============================= 
// построение словосочетаний 
//================================

bool FindField (const CDictionary* Ross, long UnitNo, string FieldStr)
{
	BYTE FieldNo = Ross->GetFieldNoByFieldStr(FieldStr.c_str());

	if (!Ross->IsEmptyArticle(UnitNo))
	{
		long UnitEndPos = Ross->GetUnitEndPos(UnitNo);
		for (size_t i = Ross->GetUnitStartPos(UnitNo); i<= UnitEndPos; i++)
			if (    (Ross->GetCortegeFieldNo(i) == FieldNo) 
				)
				return true;
	};
	return false;
};


bool IsConditional (const CRossHolder& RossDoc, long UnitNo)
{
	BYTE FieldNo = RossDoc.GetRoss()->GetFieldNoByFieldStr("TYP");

	if (!RossDoc.GetRoss()->IsEmptyArticle(UnitNo))
	{
		long UnitEndPos = RossDoc.GetRoss()->GetUnitEndPos(UnitNo);
		for (size_t i = RossDoc.GetRoss()->GetUnitStartPos(UnitNo); i<= UnitEndPos; i++)
			if (RossDoc.GetRoss()->GetCortegeFieldNo(i) == FieldNo) 
				if (RossDoc.GetRoss()->GetCortegeItem(i,0) != -1) 
				{
					string s =  RossDoc.GetDomItemStrInner(RossDoc.GetRoss()->GetCortegeItem(i,0));
					if (s == "УСЛ") return true;			
					assert (s == "БЕЗУСЛ");
					return false;
				};

	};
  return false;
};



bool CSemanticsHolder::BuildColloc (string ContentFieldStr, int CollocUnitNo)
{
	size_t i = ContentFieldStr.find("(");
	if (i == -1) 
	{
		CColloc C;
		C.UnitNo = CollocUnitNo;
		C.IsBlackBox = !FindField (GetRoss(CollocRoss), CollocUnitNo, "AUX");
		C.IsConditional = IsConditional (*GetRossHolder(CollocRoss), CollocUnitNo);
        C.m_SemMainWord =  GetRossHolder(CollocRoss)->GetSemMainWordFromArticle(CollocUnitNo);
		
		// Build items
		string ItemStr = ContentFieldStr;
		TrimLeft(ContentFieldStr);
		while (!ContentFieldStr.empty())
		{
		  int j = ContentFieldStr.find_first_of(" \t");
		  string Q = ContentFieldStr.substr (0, j);
		  ContentFieldStr.erase(0, j);
		  CCollocItem CollocItem;
		  if (!CollocItem.InitCollocItem(Q))
		  {
			  string mess = "Unbound reloperator in ";
			  mess += GetRoss(CollocRoss)->GetEntryStr(C.UnitNo);
			  ErrorMessage(mess);
		  };
		  C.Items.push_back(CollocItem);

		  TrimLeft(ContentFieldStr);
		}
		if (!GetRoss(CollocRoss)->IsEmptyArticle(CollocUnitNo))
		// по словарной статье 
		for (size_t j = GetRoss(CollocRoss)->GetUnitStartPos(CollocUnitNo); j <= GetRoss(CollocRoss)->GetUnitEndPos(CollocUnitNo); j++)
		{
		  TCortege Cort = GetCortege(GetRoss(CollocRoss), j);
		  //незаполненное поле?
		  if (Cort.m_DomItemNos[0] == -1) continue;
		  // строю массив U.m_Places по полю CONTENT
		  string FieldStr = (const char*)GetRoss(CollocRoss)->Fields[Cort.m_FieldNo].FieldStr;

		  // инициализирую перечень всех необходимых синтаксических отношений их поля SYNREP
		  if (    (FieldStr == "SYNR") 
			   && (Cort.m_LeafId == 0) 
			   && (Cort.m_BracketLeafId == 0) 
			 )
		  {
			  string F = GetRossHolder(CollocRoss)->GetDomItemStrInner(Cort.m_DomItemNos[1]);
			  long PlaceNo1 = F[1] - '0' - 1;
			  F = GetRossHolder(CollocRoss)->GetDomItemStrInner(Cort.m_DomItemNos[2]);
			  long PlaceNo2 = F[1] - '0' - 1;
			  string SynGrp = GetRossHolder(CollocRoss)->GetDomItemStrInner(Cort.m_DomItemNos[0]);
			  //rml_TRACE  (SynGrp);
			  if  (    (PlaceNo1 >= C.Items.size())
				    ||  (PlaceNo2 >= C.Items.size())
				  )
			  {
				  string mess = "Error in SYNREP  in ";
				  mess += GetRoss(CollocRoss)->GetEntryStr(C.UnitNo);
				  ErrorMessage(mess);
				  return false;
			  };
				  
			  C.m_Rels.push_back(CSynRelation(PlaceNo1, PlaceNo2, SynGrp));
		  };
		}

		m_RusCollocs.push_back(C);
	}
	else
	{
		size_t k = ContentFieldStr.find (")");
		if (k == -1)
		{
			ErrorMessage(Format ("Error in parenthesis  in colloc %s", ContentFieldStr.c_str()));
			return false;
		};

		size_t last_j = i;
        for (size_t j= i+1; j <= k; j++)
		{
          if  (    (j == k) 
			    || (ContentFieldStr[j] == '|')) 
		  {
             string q;
			 if (i >0) 
				 q += ContentFieldStr.substr(0, i-1);
			 q += string(" "); 
			 q += ContentFieldStr.substr(last_j+1, j-last_j-1);
			 q += string(" ");
		     if ( k-1 < ContentFieldStr.length() ) 
				 q += ContentFieldStr.substr(k+1);

			 if (!BuildColloc(q, CollocUnitNo) ) return false;
			 last_j = j;
		  };

		};

	};

	return true;

};


CCollocItemRefCollect* CSemanticsHolder::InsertRusCollocItemRef(string S)
{
	EngRusMakeUpper(S);
	vector<CCollocItemRefCollect>::iterator It = lower_bound(m_RusCollocItemRefs.begin(), m_RusCollocItemRefs.end(), S, LessCollocItemRefCollect());
	 if (     (It  == m_RusCollocItemRefs.end())
		 || !(It->Item  == S)
	   )
	m_RusCollocItemRefs.insert(It, CCollocItemRefCollect(S));
	It = lower_bound(m_RusCollocItemRefs.begin(), m_RusCollocItemRefs.end(), S, LessCollocItemRefCollect());
	assert (It->Item == S);
	return &(*(It));
};

bool CSemanticsHolder::BuildCollocs()
{
	long UnitsCount = GetRoss(CollocRoss)->GetUnitsSize();

	m_RusCollocs.clear();

	BYTE ContentsFieldNo = GetRoss(CollocRoss)->GetFieldNoByFieldStr("CONTENT");
	for (long UnitNo=0; UnitNo < UnitsCount; UnitNo++)
		if (!GetRoss(CollocRoss)->IsEmptyArticle(UnitNo))
			for (size_t i = GetRoss(CollocRoss)->GetUnitStartPos(UnitNo); i<= GetRoss(CollocRoss)->GetUnitEndPos(UnitNo); i++)
			if	(		(GetRoss(CollocRoss)->GetCortegeFieldNo(i) == ContentsFieldNo) 
					&&	(GetRoss(CollocRoss)->GetCortegeLeafId(i) == 0) 
				)
			{
				if (!BuildColloc (GetRossHolder(CollocRoss)->GetDomItemStrInner(GetCortege(GetRoss(CollocRoss), i).m_DomItemNos[0]), UnitNo) )
					return false;

				break;
			};


	m_RusCollocItemRefs.clear();

	for (long i=0; i < m_RusCollocs.size(); i++)
	for (long k=0; k < m_RusCollocs[i].Items.size(); k++)
	if (!m_RusCollocs[i].Items[k].IsHole())
	{

		string S =  m_RusCollocs[i].Items[k].Item;
		EngRusMakeUpper(S);
		CCollocItemRefCollect* It = InsertRusCollocItemRef(S);
		It->Refs.push_back(CCollocItemRef(i,k));

		// Добавление совершенного вида
		StringVector Vec =  GetAspVerb(m_RusCollocs[i].Items[k].Item, false);
		for (long j=0; j <Vec.size(); j++)
			if (Vec[j]  !=  S) // почему-то такое бывает? (двувидовые?)
			{
				CCollocItemRefCollect* It = InsertRusCollocItemRef(Vec[j]);
				It->Refs.push_back(CCollocItemRef(i,k));
			};
	};

	return true;
};







size_t GetCaseGrammem (const CRossHolder* RossDoc, long ItemNo) 
{
  if (ItemNo == RossDoc->NominativeNo) 
     return _QM(rNominativ);
  else if (ItemNo ==  RossDoc->GenitivNo)
	 return _QM( rGenitiv);
  else if (ItemNo ==  RossDoc->DativNo)
     return _QM( rDativ);
  else if (ItemNo ==  RossDoc->VocativNo)
	 return _QM( rLocativ);
  else if (ItemNo ==   RossDoc->AccusativNo )
	 return _QM( rAccusativ);
  else if (ItemNo ==  RossDoc->InstrumentalisNo)
     return _QM( rInstrumentalis);
  else if (ItemNo == RossDoc->NominativePluralisNo) 
     return _QM(rNominativ) | _QM(rPlural);
  else if (ItemNo ==  RossDoc->GenitivPluralisNo)
	 return _QM( rGenitiv) | _QM( rPlural);
  else if (ItemNo ==  RossDoc->DativPluralisNo)
     return _QM( rDativ) | _QM( rPlural);
  else if (ItemNo ==  RossDoc->VocativPluralisNo)
	 return _QM( rLocativ) | _QM( rPlural);
  else if (ItemNo ==   RossDoc->AccusativPluralisNo )
	 return _QM( rAccusativ) | _QM( rPlural);
  else if (ItemNo ==  RossDoc->InstrumentalisPluralisNo)
     return _QM( rInstrumentalis) | _QM( rPlural);
  else if (ItemNo == RossDoc->NominativeSingularNo) 
     return _QM(rNominativ) | _QM(rSingular);
  else if (ItemNo ==  RossDoc->GenitivSingularNo)
	 return _QM( rGenitiv) | _QM( rSingular);
  else if (ItemNo ==  RossDoc->DativSingularNo)
     return _QM( rDativ) | _QM( rSingular);
  else if (ItemNo ==  RossDoc->VocativSingularNo)
	 return _QM( rLocativ) | _QM( rSingular);
  else if (ItemNo ==   RossDoc->AccusativSingularNo )
	 return _QM( rAccusativ) | _QM( rSingular);
  else if (ItemNo ==  RossDoc->InstrumentalisSingularNo)
     return _QM( rInstrumentalis) | _QM( rSingular);
  else
     return 0;
};

size_t GetCaseItemNo (CRossHolder* RossDoc, long GrammemNo) 
{
 if (GrammemNo == rNominativ) 
     return RossDoc->NominativeNo;
  else if (GrammemNo ==  rGenitiv)
	 return RossDoc->GenitivNo;
  else if (GrammemNo ==  rDativ)
     return RossDoc->DativNo;
  else if (GrammemNo ==  rLocativ)
	 return RossDoc->VocativNo;
  else if (GrammemNo ==   rAccusativ )
	 return RossDoc->AccusativNo;
  else if (GrammemNo ==  rInstrumentalis)
     return RossDoc->InstrumentalisNo;

  return 0;
};

bool IsCase (const CRossHolder* RossDoc, long ItemNo) 
{
 if (ItemNo == -1) return false;

 return    (ItemNo == RossDoc->NominativeNo)
	    || (ItemNo == RossDoc->GenitivNo)
		|| (ItemNo == RossDoc->DativNo)
		|| (ItemNo == RossDoc->VocativNo)
		|| (ItemNo == RossDoc->AccusativNo)
		|| (ItemNo == RossDoc->InstrumentalisNo)

		|| (ItemNo == RossDoc->NominativePluralisNo)
	    || (ItemNo == RossDoc->GenitivPluralisNo)
		|| (ItemNo == RossDoc->DativPluralisNo)
		|| (ItemNo == RossDoc->VocativPluralisNo)
		|| (ItemNo == RossDoc->AccusativPluralisNo)
		|| (ItemNo == RossDoc->InstrumentalisPluralisNo)

		|| (ItemNo == RossDoc->NominativeSingularNo)
	    || (ItemNo == RossDoc->GenitivSingularNo)
		|| (ItemNo == RossDoc->DativSingularNo)
		|| (ItemNo == RossDoc->VocativSingularNo)
		|| (ItemNo == RossDoc->AccusativSingularNo)
		|| (ItemNo == RossDoc->InstrumentalisSingularNo)
		;
};


