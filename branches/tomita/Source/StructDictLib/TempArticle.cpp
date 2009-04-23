// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Structural Dictionary (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (1998-2002)

#include "StdRoss.h"
#include "StructDictConsts.h"
#include "TempArticle.h"
#include "../common/util_classes.h"



CTempArticle::CTempArticle()
{
	m_pRoss = NULL;
	m_ReadOnly = false;
	m_UnitNo = ErrUnitNo;
	m_ErrorLine = -1;
}


const TCortege10& CTempArticle::GetRossCortege (size_t i) const
{
	const TCortege10& C = *m_pRoss->GetCortege(i);
	return C;
};

size_t CTempArticle::GetCortegesSize ()const
{
	if (m_ReadOnly)
		if ((m_UnitNo != ErrUnitNo) && (m_pRoss->m_Units[m_UnitNo].HasEmptyArticle()))
			return 0;
  		else
				return m_pRoss->m_Units[m_UnitNo].m_LastCortegeNo -  m_pRoss->m_Units[m_UnitNo].m_StartCortegeNo + 1;
	else
		return m_Corteges.size();
};

const TCortege10& CTempArticle::GetCortege (size_t i)  const
{
		if (m_ReadOnly)
			return GetRossCortege(m_pRoss->m_Units[m_UnitNo].m_StartCortegeNo+i);
		else
			return m_Corteges[i];
};


string CTempArticle::ConstructFldName (BYTE FieldNo, BYTE LeafId, BYTE BracketLeafId)
{	
	string s(m_pRoss->Fields[FieldNo].FieldStr);
	
	if (LeafId > 0)
		s += (LeafId == ErrUChar) ? '*':('0'+LeafId);

	if (BracketLeafId > 0)
		{
			s += '(';
			s += (BracketLeafId == ErrUChar) ? '*':('0'+BracketLeafId);
			s += ')';
		};
		
	return s;
};

int CTempArticle::IntersectByFields(const CTempArticle *Article) const
{

    size_t sum =0;

	for (size_t i=0; i < GetCortegesSize(); i++)
      for  (size_t k=0; k<Article->GetCortegesSize(); k++)
	   if  (   (GetCortege(i).m_FieldNo == Article->GetCortege(k).m_FieldNo)
		     && (GetCortege(i).m_LeafId == Article->GetCortege(k).m_LeafId)
			 && (GetCortege(i).m_BracketLeafId == Article->GetCortege(k).m_BracketLeafId)
           )
       sum++;

    return sum;
}

bool CTempArticle::AddArticle(const CTempArticle *Article)
{
	assert (!m_ReadOnly) ;

	for  (size_t k=0; k<Article->GetCortegesSize(); k++)
	{
		size_t i=0;
		for (; i< GetCortegesSize(); i++)
			if (GetCortege(i).EqualCortege(Article->GetCortege(k), m_pRoss->m_MaxNumDom)  )
			 break;

	        if (i == GetCortegesSize())
			m_Corteges.push_back(Article->GetCortege(k));
	};

	if (!CheckCortegeVector()) return false;

	return true;
}


bool CTempArticle::IsPartOf(const CTempArticle *Article, bool UseWildCards) const
{
	for (size_t i=0; i<GetCortegesSize(); i++)
	{
		size_t k=0;

		for (; k < Article->GetCortegesSize(); k++)
			if (    (    !UseWildCards
			        && (GetCortege(i).EqualCortege(Article->GetCortege(k), m_pRoss->m_MaxNumDom)) )
				|| (    UseWildCards
			         && GetCortege(i).IsEqualWithWildCard(Article->GetCortege(k), m_pRoss->WildCardDomItemNo, m_pRoss->m_MaxNumDom )
				   )
				) 
			  break;

		if (k == Article->GetCortegesSize())
			return false;
	  };

    
	return true;
}


const size_t MaxOfTheFirstPart = 200;

inline int  VisualFieldOrder (BYTE LeafId, BYTE BracketLeafId, int OrderId)
{
  if ( (LeafId == 0) && (BracketLeafId == 0))
    return OrderId;
  else  
    return OrderId+MaxOfTheFirstPart+LeafId*MaxOfTheFirstPart+BracketLeafId;
}

bool   CTempArticle::PutCortegeOnTheRigthPosition (const TCortege10& C)
{
	  // Ќомер места получим в переменной i. ¬начале переменна€ i равна нулю.
	// ѕроходим все пол€, у которых пор€док меньше пор€дка текущего пол€
	size_t i = 0;
	for  (;
                 (i < GetCortegesSize()) 
		     && (VisualFieldOrder(C.m_LeafId,C.m_BracketLeafId,m_pRoss->Fields[C.m_FieldNo].OrderId) >
                 VisualFieldOrder(GetCortege(i).m_LeafId, GetCortege(i).m_BracketLeafId, m_pRoss->Fields[GetCortege(i).m_FieldNo].OrderId));
           i++);

      //  ѕроходим записи, которые относ€тс€ к данному полю, чтобы дописать текущую запись
      //  после последней.
      //  ќпираемс€ на то, что OrderId - уникален
    for  (;
                  (i < GetCortegesSize()) 
               && (C.m_LeafId   == GetCortege(i).m_LeafId) 
			   && (C.m_BracketLeafId   == GetCortege(i).m_BracketLeafId) 
               && (C.m_FieldNo  == GetCortege(i).m_FieldNo)  
               && (C.m_LevelId  >= GetCortege(i).m_LevelId);
           i++);

       
    // ƒобавление  к списку значений
	m_Corteges.insert(m_Corteges.begin()+i, C);

	return true;
};


bool CTempArticle::ArticleToText ()
{
   int CurrLevelId = 0;
   BYTE CurrLeafId = ErrUChar;
   BYTE CurrBracketLeafId = ErrUChar;
   BYTE CurrFieldNo = ErrUChar;
   string LeftPart;
   char RightPart[2000];
   char Line[2000];
   string S; 
   m_ArticleStr = "";
   
   
   for (size_t i= 0; i < GetCortegesSize();  i++)
    {
	 TCortege10 C =  GetCortege(i);
	 RightPart[0] = 0;
     
     // смена текущей функции - значит, смена уровн€
     if (    (CurrLeafId != C.m_LeafId) 
		  || (CurrBracketLeafId != C.m_BracketLeafId) 
		  || (C.m_FieldNo != CurrFieldNo) 
        )
          CurrLevelId = 0;

     if (C.m_LevelId > 0) 
       if  (C.m_LevelId > CurrLevelId)
	   {
		   char s[20];
           CurrLevelId = C.m_LevelId;
		   if (C.m_LevelId == ErrUChar) 
			   strcpy (s, "*");
		   else
			   sprintf (s, "%i",C.m_LevelId);
		   strcpy (RightPart, s);
		   strcat (RightPart," ");
       }
	   else 
		   strcpy(RightPart, "  ");

	   m_pRoss->CortegeToStr(C, RightPart + strlen(RightPart));

      // пошло новое поле
       if (    (CurrFieldNo != C.m_FieldNo)
	 	    || (CurrLeafId != C.m_LeafId)
			|| (CurrBracketLeafId != C.m_BracketLeafId)
		  )
	   {
         LeftPart = ConstructFldName (C.m_FieldNo, C.m_LeafId, C.m_BracketLeafId);

		 sprintf (Line, C.GetFieldFormat(), LeftPart.c_str(), RightPart);
         CurrLevelId = C.m_LevelId;
         CurrLeafId  = C.m_LeafId;
		 CurrBracketLeafId  = C.m_BracketLeafId;
		 CurrFieldNo =  C.m_FieldNo;
	   } 
       else
       // продолжаетс€ запись значени€ старого пол€
        sprintf (Line, "%10s%s"," ", RightPart);

        m_ArticleStr += Line;
	    m_ArticleStr += "\n"; 
	};
  return true;
};

void DeleteEmptyLines(string& s)
{
	for (int i = 0; i < s.length(); i++)
	  while (     (s[i] == '\n') 
		   &&  (i < s.length() - 1)
		 )
	  {
		int k = i+1;

		for (; k<s.length(); k++)
			if (   (s[k] != ' ') 
				&& (s[k] != '\t') 
				&& (s[k] != '\r')
			   )
			break;

		if ((k == s.length()) || (s[k] == '\n'))
			s.erase(i+1,k+1-(i+1)); 
		else
			break;
	  };
};


string GetLine(string s, size_t LineNo)
{
	size_t  start = 0;
	for (size_t i =0; i < LineNo; i++)
	{
		start = s.find('\n', start) + 1;
	};

	size_t end = s.find('\n', start); 
	if (end == s.npos)
		end = s.length();

	return s.substr(start, end - start);
};


bool CTempArticle::AddCortegeToVector (CTextField& F)
{

BYTE LevelId = 0;

bool IsEqual = false;

m_LastError = "";
m_ErrorLine = -1;

for (size_t i= F.StartLine; i <= F.EndLine; i++)
{
	string s = GetLine(m_ArticleStr, i);
	int  j = s.length() - 1;
	for (  ;
	      (j >= 0)  && isspace((unsigned char)s[j]);
	  j--);
        j++;

	s.erase(s.begin() +j, s.end());
		
        
	if (i == F.StartLine)
	{
		int p = s.find('=');
		s.erase(0, p + 1);
		if ((unsigned char)s[0] == '=')
        	{
	             s.erase(0, 1);
	             IsEqual = true;
		};


	};



	int k = s.find_first_not_of(" \t");
	if (k != s.npos)
		s.erase(0, k);
	else
		s = "";



	string FieldStr = ConstructFldName (F.FieldNo, F.LeafId, F.BracketLeafId);
		
	if   (     (m_pRoss->Fields[F.FieldNo].TypeRes == frFormula)
	    &&  !s.empty())
	{
		int j =  atoi (s.c_str());
		if  (    (j > 0) 
				  || (s[0] == '*')
				)
		{
			LevelId = (s[0] == '*') ? ErrUChar : j;
			int k = s.find(' ');
			if (k == string::npos)
			{
				s = "";
			}
			else
			{
				s.erase (0, k+1);
			        s.erase(0, s.find_first_not_of(" \t"));
			};
		};

		if    (LevelId < 1)  
		{
			m_LastError = Format ("Error: No level number (field %s)",  (const char*)FieldStr.c_str());
			m_ErrorLine = i + 1;
			return false;
		};

		if  (    (LevelId > 40) 
			  && (LevelId != ErrUChar)) 
		{
			m_LastError = Format ("Error: Level number is too large (field %s) ", (const char*)FieldStr.c_str());
			m_ErrorLine = i + 1;
			return false;
		};


	}; 

	TCortege10 C;

	C.m_FieldNo = F.FieldNo;		
	C.m_LeafId = F.LeafId;
	C.m_BracketLeafId = F.BracketLeafId;
	C.m_LevelId = LevelId;
	if (   !s.empty() )
	{
		bool b;
		b = m_pRoss->ReadFromStr (s.c_str(), C);
		if (IsEqual) 
			C.SetEqual();
		if (!b)
		{
			m_LastError += Format ("\nCannot read line  (\"%s\")! ",  s.c_str());
			m_ErrorLine = i + 1;
			return false;
		 }
	};;

	if ((C.m_LevelId == 0) &&  (m_pRoss->Fields[F.FieldNo].TypeRes == frFormula))
			C.m_LevelId = 1; 

	 m_Corteges.push_back(C);
};

return true;

};


bool CTempArticle::CheckCortegeVector ()
{

	int ActantNo = 1;
	for (size_t i=0; i<GetCortegesSize(); i++)
	{
		m_ErrorLine = i+1;
		if (GetCortege(i).m_FieldNo == ErrUChar) 
		{
			m_LastError = "Unknown field ";
			return false;
		};


		size_t k=0;
		for (; k < m_pRoss->Fields[GetCortege(i).m_FieldNo].m_Signats[GetCortege(i).GetSignatNo()].Doms.size(); k++) 
			if  (GetCortege(i).GetItem(k) == -1)
			{
				m_LastError = "Empty field";
				return false;
			}

			string FldName = ConstructFldName(GetCortege(i).m_FieldNo, GetCortege(i).m_LeafId, GetCortege(i).m_BracketLeafId);

			bool FoundPrevLevelId = !(GetCortege(i).m_LevelId > 1 && (GetCortege(i).m_LevelId != ErrUChar));
			for (k=0; k<GetCortegesSize(); k++)
				if (    (i!= k)
					&& (GetCortege(i).m_FieldNo == GetCortege(k).m_FieldNo)
					&& (GetCortege(i).m_LeafId == GetCortege(k).m_LeafId)
					&& (GetCortege(i).m_BracketLeafId == GetCortege(k).m_BracketLeafId)
					)
				{
					if (m_pRoss->Fields[GetCortege(i).m_FieldNo].TypeRes == frOne)
					{
						m_LastError =  Format ("Field \"%s\" is repeated", FldName.c_str());
						return false;
					};

					if (!FoundPrevLevelId)
						if (GetCortege(k).m_LevelId == GetCortege(i).m_LevelId - 1)
							FoundPrevLevelId = true;
				};

			if (!FoundPrevLevelId)
			{
				m_LastError =  Format ("Bad level numbers in \"%s\"", FldName.c_str());
				return false;
			};


			for (k=0; k<GetCortegesSize(); k++)
			{
				if (    (i!= k)
					&& (GetCortege(i).EqualCortege(GetCortege(k), m_pRoss->m_MaxNumDom))
					&& (m_pRoss->Fields[GetCortege(i).m_FieldNo].TypeRes == frOne)
					)
				{
					m_LastError =  Format ("Two equal items on one level in \"%s\"",FldName.c_str());
					return false;
				};


			};


			if (strcmp(FldName.c_str(),"VAL") == 0)
			{
				char q[200] = "\0";

				const char* s = m_pRoss->GetDomItemStrInner( GetCortege(i).GetItem(1));
				strcat (q, s);
				s = m_pRoss->GetDomItemStrInner( GetCortege(i).GetItem(2));
				strcat (q, s);
				int l = 0;
				if (strlen (q) == 3)
					if (    (isdigit((unsigned char)q[1])  && (ActantNo+'0' != q[1]))
						|| (isdigit((unsigned char)q[2])  && (ActantNo+'0' != q[2]))
						)
			  {
				  m_LastError = Format ("Error in valency numbering (field \"%s\")",FldName.c_str());
				  return false;
			  };

				ActantNo++;
			};

	}



	return true;
}


inline bool ProcessBracketLeafId(string& FldName, BYTE& BracketLeafId)
{
	int i = FldName.length() - 2;

 	for (; i >0; i--)
		if (!isspace((BYTE)FldName[i]))
				break;

    if (i == 0) return false;

	if (isdigit((BYTE) FldName[i])) 
	   BracketLeafId = FldName[i] - '0';
	else
		if (FldName[i] == '*')
			BracketLeafId = ErrUChar;
		else return false;

	for (i--; i>0; i--)
	  if (!isspace((BYTE)FldName[i]))
      	break;

   if (i == 0) return false;

   if  (  ((BYTE)FldName[i]) != '(') return false;

   for (i--; i >0; i--)
		if (!isspace((BYTE)FldName[i]))
			break;
   if (i == 0) return false;
   FldName.erase(i+1);
   return true;
};





inline bool SplitFldName (string& FldName, BYTE& LeafId, BYTE& BracketLeafId)
{
	BracketLeafId = 0;

	if (FldName[FldName.length() - 1] == ')')
		ProcessBracketLeafId(FldName, BracketLeafId);

	BYTE debug = FldName.length();
	LeafId = FldName[FldName.length() - 1] - '0';

	if (     (    (LeafId > 9) 
		       || (LeafId == 0))
		 && (FldName[FldName.length() - 1] != '*')
	   )
		 LeafId = 0;
    else
	{
		if (FldName[FldName.length() - 1] == '*')
			LeafId = ErrUChar;
	    FldName[FldName.length() -1] = 0;
	};

	return true;
};



inline bool IsPartOf (const vector<TCortege10>& L1,  const vector<TCortege10>& L2, bool UseWildCards, int EmptyDomItemNo, BYTE MaxNumDom)
{
  for (size_t i=0; i<L1.size(); i++)
  {
	size_t k=0;
	 
	for (; k < L2.size(); k++)
		  if (    (    !UseWildCards
			        && (L1[i].EqualCortege(L2[k], MaxNumDom)))
				|| (    UseWildCards
			         && L1[i].IsEqualWithWildCard(L2[k], EmptyDomItemNo, MaxNumDom)
				   )
			 ) 
			  break;

      if (k == L2.size())
		  return false;
  };
  return true;
};

inline bool AreEqual (const vector<TCortege10>& L1,  const vector<TCortege10>& L2, int MaxNumDom)
{
	return IsPartOf (L1, L2, false,0, MaxNumDom) && IsPartOf (L2, L1, false, 0, MaxNumDom);
};


void CTempArticle::ReadFromDictionary(WORD UnitNo, bool VisualOrder, bool ReadOnly)
{
	m_UnitNo = UnitNo;
	m_ReadOnly = ReadOnly ? true : false;
	m_Corteges.clear();
	const CStructEntry& U  =  m_pRoss->GetUnits()[UnitNo];
	strcpy (m_EntryStr,U.m_EntryStr);
	m_MeanNum = U.m_MeanNum;



	if (!m_ReadOnly)
		if ( !m_pRoss->GetUnits()[UnitNo].HasEmptyArticle() )
			for (int i= m_pRoss->GetUnits()[UnitNo].m_StartCortegeNo; i <= m_pRoss->GetUnits()[UnitNo].m_LastCortegeNo; i++)
				if (VisualOrder)
					PutCortegeOnTheRigthPosition (GetRossCortege(i));
				else
					m_Corteges.push_back(GetRossCortege(i));

}


bool CTempArticle::SetArticleStr(const char * s)
{
	m_ArticleStr = "";
	m_ArticleStr += s; 
	DeleteEmptyLines(m_ArticleStr);
	if (!MarkUp()) return false;
	if (!BuildCortegeList()) return false;;
	return true;
}


bool  CTempArticle::MarkUp()
{
	if (m_ReadOnly) 
	{
		m_LastError = "Article is readonly";
		m_ErrorLine = -1;
		return false;
	};

	m_Fields.clear();
	size_t LineNo = 0;
	int  old_eoln = -1;
	size_t Size = m_ArticleStr.length();
	if (m_ArticleStr.empty()) return true;
	for (size_t _eoln =0; _eoln <= Size;  _eoln++)
	if ((_eoln == Size) ||  (m_ArticleStr[_eoln] == '\n'))
	{
		if ( (_eoln == Size)  && (old_eoln+1 ==  _eoln)) continue;
		string Line,FldName,Delim;
		Line = m_ArticleStr.substr(old_eoln+1, _eoln-old_eoln-1);
		StringTokenizer tok (Line.c_str(), " \t");
		if (tok())
			FldName = tok.val();
		if (tok())
			Delim = tok.val();
		bool IsGoodField = (!Delim.empty() && (Delim[0] == '=')); 

		//  checking if  field name and "=" were written without space
		if (		!IsGoodField
				&& !FldName.empty()
				&& (FldName[FldName.length() -1] == '=')
		 )
		{
			FldName.erase(FldName.length() - 1);
			IsGoodField = true;
		};

		if (IsGoodField)
		{

			BYTE LeafId, BracketLeafId;
			SplitFldName (FldName, LeafId,BracketLeafId);
			BYTE  FieldNo =  m_pRoss->GetFieldNoByFieldStrInner (FldName.c_str());
			if (FieldNo == ErrUChar)
			{ 
				m_LastError = Format("Field \"%s\"  is not registered", FldName.c_str());
				m_ErrorLine = LineNo+1;
				m_Fields.clear();
				return false;
			}

			CTextField T;
			T.StartLine = LineNo;
			T.EndLine = LineNo;
			T.FieldNo = FieldNo;
			T.LeafId = LeafId;
			T.BracketLeafId = BracketLeafId;
			m_Fields.push_back(T);
		}
		else
			if (!m_Fields.empty())
				m_Fields[m_Fields.size() - 1].EndLine ++;
			else
		 {
			 m_LastError = Format("Field \"%s\"  is not registered", FldName.c_str());
			 m_ErrorLine = LineNo+1;
			 m_Fields.clear();
			 return false;

		 };

		LineNo ++;
		old_eoln = _eoln;
	};
	return true;
}




bool CTempArticle::BuildCortegeList()
{
	if (m_ReadOnly) 
	{
		m_LastError = "Article is readonly";
		return false;
	};
	size_t DomItemCount = m_pRoss->m_DomItems.size();
	m_Corteges.clear();


	for (size_t i=0; i < m_Fields.size(); i++)
	if  (!AddCortegeToVector (m_Fields[i]))
	{
		m_Corteges.clear();
		return false;
	};

	/*
		если в процессе парсинга были добавлены новые константы, тогда
		нужно пропарсить еще раз, поскольку все ссылки в векторе Corteges сбились
	*/
	if (DomItemCount != m_pRoss->m_DomItems.size() )
	{
		m_Corteges.clear();
		for (size_t i=0; i < m_Fields.size(); i++)
		if  (!AddCortegeToVector (m_Fields[i]))
		{
			m_Corteges.clear();
			return false;
		}

	};

	return true;
}


bool CTempArticle::WriteToDictionary()
{
	if (m_ReadOnly) 
	{
		m_LastError = "Article is readonly";
		m_ErrorLine = -1;
		return false;
	};

    if (!CheckCortegeVector()) return false;

	vector<CStructEntry>& Units = m_pRoss->m_Units;

	if ( !m_pRoss->m_Units[m_UnitNo].HasEmptyArticle() )
	     m_pRoss->DelCorteges (Units[m_UnitNo].m_StartCortegeNo, Units[m_UnitNo].m_LastCortegeNo + 1);

	Units[m_UnitNo].m_StartCortegeNo = m_pRoss->_GetCortegesSize();

	int i = 0;

	for (; i<GetCortegesSize(); i++)
	{
		TCortege10 C;
		C = GetCortege(i);
		m_pRoss->_AddCortege(C);
	};


	Units[m_UnitNo].m_LastCortegeNo = m_pRoss->_GetCortegesSize()-1;

	if  ( GetCortegesSize() == 0)
	{
		Units[m_UnitNo].m_LastCortegeNo = InitialEndPos;
		Units[m_UnitNo].m_StartCortegeNo = InitialStartPos;
	};

	return true;
}



bool CTempArticle::IsModified() const
{

    vector<TCortege10> SavedArticle;
	int StartPos = m_pRoss->m_Units[m_UnitNo].m_StartCortegeNo;
	int EndPos   = m_pRoss->m_Units[m_UnitNo].m_LastCortegeNo;

	if (!m_pRoss->m_Units[m_UnitNo].HasEmptyArticle() )
      for (size_t i=StartPos; i <= EndPos; i++)
	  {
			SavedArticle.push_back(GetRossCortege(i));
	  };

	return !AreEqual (m_Corteges, SavedArticle, m_pRoss->m_MaxNumDom);

}

const string& CTempArticle::GetArticleStr()
{
    ArticleToText();
	return m_ArticleStr;
}



