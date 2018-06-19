// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko

#include "Sentence.h"
#include "FormatCaller.h"
#include "../AgramtabLib/rus_consts.h"


class CWord_eq : public unary_function<CSynWord,bool>
{
	string s;
public:
	CWord_eq(string ss) : s(ss) {}
	bool operator() (CSynWord& el)  
	{ return el.m_strWord ==  s; }
};


static int GetConjProperties(const CSentence& S, int WordNo, ConjFromWhatList& FromWhere,int& iConjIndex, int UpperBorder)
{
	if( (WordNo == -1) || (WordNo >= UpperBorder) )
		return -1;

	int OborotNo = S.m_Words[WordNo].GetOborotNo();

	if	(		OborotNo != -1 
			&& 	S.GetOpt()->GetOborDic()->m_Entries[OborotNo].HasPartOfSpeech(S.GetOpt()->m_Conjunction)
		)
	{
		for(size_t i = WordNo ; i < UpperBorder; i++ )	
			if( S.m_Words[i].HasOborot2())
			{
				FromWhere = FROM_OBOR_DIC;
				iConjIndex = OborotNo;
				return WordNo + 1;
			}
	}

	iConjIndex = S.m_Words[WordNo].m_SubordinateConjNo;
	int HomonymNo;
	if	(		(iConjIndex != -1 )
			&&	!S.IsRelativSentencePronoun(0, WordNo, HomonymNo)
		)
	{
		FromWhere = FROM_SUB_CONJ;
		return WordNo+1;
	}

	return -1;
}


static int PassGraphematicalOborot(const CSentence& S, int iW)
{
	for(int i = 0 ; i < S.m_vectorGraPairs.size() ; i++ )
	{
		const SGraPair& P = S.m_vectorGraPairs[i];
		if	(		(P.m_type == Date) 
				||	(P.m_type == WebAddr) 
				||	(P.m_type == Keyb)
				||	(P.m_type == GermanDividedCompound)
			)
			if	(		(P.m_iW1 <= iW) 
					&&	(P.m_iW2 >= iW) 
				)
				return P.m_iW2;

	}	

	return -1;
}


/*
  ��� ������� ���������� �� ������� IsClauseBorder.
  �� ������������� ������ ���� ���� (), {}, <>, ���� ������ ��� ���� �����, 
  � ������� ���� ������������� �������,
  ����� ����� ������ �� ��������� ������������ �����.

  m_bFirstInPairFound - ��������, ��� ����� WordNo ����� ����� ����������� ������
  � �� ����������� ������, ������ ����������� ������ ���� ������������ ������ 
  ����� ������ �� �����������, ����� ������� ��� ������������  ������.

  ������� ���������� -1, ���� ������ ������ �� �������� ������������ ������.
*/
const char OpenBracket[] = "(<[{";
const char CloseBracket[] = ")>]}";

int CSentence::CheckIfBracketIsAClauseBorder(int WordNo, int& iStartSearch)
{
	const CSynWord& pWord = m_Words[WordNo];	
	if( !m_bFirstInPairFound )
	{
		const char* open_brck = strchr (OpenBracket,  pWord.m_strWord[0]);
		if(open_brck  != 0)
		{
			char close_brck[2];
			close_brck[0] =  CloseBracket[open_brck-OpenBracket];
			close_brck[1] =  0;
			CWordVector::const_iterator word = microsoft_find_if(m_Words.begin() + WordNo + 1,m_Words.end(),CWord_eq(close_brck));			
			if( word != m_Words.end())
			{
				long Distance =  (word - m_Words.begin()) - WordNo;
				/*
				  ����  � ������ ����� ����� ������ ����� 
				  ��� �����, ������� ����� � ������� - ������������� ��������,
				  ����� ����� ������� ��� ����������� ������ ������� ������
				*/
				if  (  (Distance >	 2 )
					 || AllHomonymsArePredicates(m_Words[WordNo+1])
					)
				{						
					iStartSearch = WordNo+1;
					m_bFirstInPairFound = true;
					return WordNo-1;
				}
				else
				{
					iStartSearch = (word - m_Words.begin()) + 1;
					if ( iStartSearch == m_Words.size() )
						--iStartSearch;
					return -1;
				}
			}
			else
			{
				iStartSearch = WordNo+1;
				return -1;
			}
		}
	}
	else
	{
		const char* close_brck = strchr (CloseBracket,  pWord.m_strWord[0]);

				
		if(close_brck != 0)
		{
			// ����� �� ������ 
			m_bFirstInPairFound = false;

			// ����������� ������, ������� ����� ����� ������ ����������� ����� ������������, 
			// ��������� ����� ������  ������ ������ � ������ �����������. ����� ��������� ������, ������� 
			// ������� ������ �� ����� ����������� (����� ����������).
			// ������: "� ��� (����)."
			if (		(WordNo+2 == m_Words.size())
                    &&	m_Words[WordNo+1].HasDes(OPun)
				)
			{
				iStartSearch = WordNo+1;
				return -1;
			};

			// ������ ���� ����� ������ 
			iStartSearch = WordNo+1;
			return WordNo;
		}
	}
	iStartSearch = WordNo+1;
	return -1;
}

/*
  ��  ���� �������� �����, �����, ������� �������� �������.
  ��� �������  ���������, ��� ������ ��������� ���������� ������
  ������ ���� �������. ���� �����, ���������� ����� ���������� ����� 
  ���� ������, ���� ��� ���������� -1.

  ������ ������ �� ����� ��������� ���������� �������, ������ ���������� ��-��
  �������:
	 "� ��� ���� �����, ������� �� ����"
	 (����� ������� �� ���������� �������� ���� ������� �� InitSimpleSimilarRules)

  ����������, ��� ������� � ������� ���������� �����, �� �����_�� ����� �� �����������.

*/


int CSentence::CanLinkSimpleSimilar(int CommaWordNo) 
{
	try
	{

		/*
			if comma is at the very  beginning of ath the end, then exit
		*/
		if	(		(CommaWordNo == 0) 
				||	(CommaWordNo + 1 >= m_Words.size() )
			)
			return -1;


		if (GetOpt()->m_Language == morphGerman)
		{
			// we can use CSentence::m_GroupsUnion if Tomita is enabled
			for (size_t i=0; i< m_GroupsUnion.GetGroups().size(); i++)
			{
				const CGroup& group = m_GroupsUnion.GetGroups()[i];
				//  ignore groups which contain only three words and the second word is 
				// a comma (a clause delimiter)
				if (group.size() == 3)
					if (m_Words[group.m_iFirstWord+1].m_bComma)
						continue;

				if	(		(group.m_iFirstWord < CommaWordNo) 
						&&	(group.m_iLastWord > CommaWordNo)
					)
					return group.m_iLastWord;
			};
			return -1;
		};


		const int Radius = (GetOpt()->m_Language == morphGerman)? 10 : 6;
		int StartClauseWordNo = max(0, CommaWordNo - Radius);
		CSentence* pSent = GetOpt()->NewSentence();
		if (!pSent)
			throw CExpc ("Cannot create sentence");
		
		for (int i = StartClauseWordNo; i < min((int)m_Words.size(), CommaWordNo + Radius); i++)
            pSent->m_Words.push_back(m_Words[i]);
		

		CClause C(pSent, 0,  pSent->m_Words.size() - 1);
		pSent->AddClause(C);	
		pSent->m_bShouldUseTwoPotentialRule = false;
		pSent->RunSyntaxInClauses(SimpleSimilarRules);
		
		int Result = -1;
		const CClause& prClause = pSent->m_Clauses[0];
		for (CSVI it = prClause.m_SynVariants.begin(); (Result == -1)&& (it!=prClause.m_SynVariants.end()); it++)
			for (size_t i=0; i< it->m_vectorGroups.GetGroups().size(); i++)
			{
				const CGroup& group = it->m_vectorGroups.GetGroups()[i];
				//  ignore groups which contain only three words and the second word is 
				// a comma (a clause delimiter)
				if (group.size() == 3)
				{
					const CSynUnit& U = it->m_SynUnits[group.m_iFirstWord+1];
					if (pSent->m_Words[U.m_SentPeriod.m_iFirstWord].m_bComma)
						continue;
				};

				if	(		(group.m_iFirstWord+StartClauseWordNo < CommaWordNo) 
						&&	(group.m_iLastWord+StartClauseWordNo > CommaWordNo)
					)
				{
					Result = group.m_iLastWord + StartClauseWordNo;
					break;
				};
			}
		
		delete pSent;

		return Result;
	}
	catch(...)
	{
		OutputErrorString("Failed RunSyntaxInClause(CanLinkSimpleSimilar)");
		return -1;
	}

}

class CWordInPair_eq : public unary_function<SFoundTermin, bool>
{
public:
	CWordInPair_eq(int iWord) { m_iWord = iWord; };
	bool operator() (SFoundTermin& _pair)
	{
		return ( m_iWord > _pair.m_iFirstWord ) && (m_iWord < _pair.m_iLastWord );
	}

private:
	int m_iWord;
};



/*
 iPunctsCount - ����� ��������������� ����, � ������� ����� ����� ����������
 ��� ����� ���������� ��������� ������
 WordNo - ����� �����, ��� �������� �����������, ����� �� ��� ���� ����������� �����
 iStartSearch - ����� �����, � �������� ���� �������� ����� ������ ����� ������
 iFWrd - ������ ������� ������
 ������� ����������  ����� �����, ������� ������ ������ ������, ����  -1 (���� ����� WordNo �� ���� ������� ����� ������)
 ����� �����. �������� �� ����� -1, WordNo ����� �� ���������  � ������������ ��������, ������ ���, ��������,
 WordNo ����� ������ "��������" ����� ������.
*/


int CSentence::IsClauseBorder(int WordNo, int& iStartSearch,int& iPunctsCount, int iFWrd)
{
	assert( (WordNo >= 0) && (WordNo < m_Words.size()) );

	const CSynWord& pWord = m_Words[WordNo];


	if (GetOpt()->m_Language == morphRussian)
	{	// a clause border cannot divide an analytical verb form (for Russian)
		int  NextWord  = FindFirstAuxVerb(WordNo);
		if (!pWord.m_MainVerbs.empty())
			NextWord = max( NextWord, *max_element(pWord.m_MainVerbs.begin(), pWord.m_MainVerbs.end()));
		if (NextWord > WordNo)
		{
			iStartSearch = NextWord;
			return -1;
		};
    };
	// ���� �� ����� �� ����� �����������
 	if( WordNo+1 == m_Words.size() )
	{
		// ����� ����������� ������ ���� ����������
		iPunctsCount = 1;
		iStartSearch = WordNo+1;
		//���� ������� ��� ��������� � ����� ������, ����� �������� ������� �� ������� ���������
		if (SetClauseBorderIfThereAreTwoPotentialPredicates(iFWrd, WordNo))
		{
			iStartSearch = WordNo;
			return WordNo-1;
		}
		return WordNo;
    }


	// ������� ������ �� ����� ��������� ����� ������, ���� ������� ����� ������
	if( microsoft_find_if(m_vectorTermins.begin(), m_vectorTermins.end(), CWordInPair_eq(WordNo)) != m_vectorTermins.end())
	{		
		iStartSearch = WordNo + 1;
		return -1;				
	}

	// ��� ��������������� �����, ����� ��������, ����� ������. ��������������� �����: web-������, ����.
	if (WordNo+1 != m_Words.size())
	{
		int iLast = PassGraphematicalOborot(*this, WordNo);
		if (iLast != -1) 
		{
			if( iLast + 1 < m_Words.size() )
				iStartSearch = iLast + 1;
			else
				iStartSearch = iLast;
			return -1;
		}
    };

	
	// ���� ���������� - �������������   ����������� ����� - ����� ��� ���������
    if( pWord.HasDes(OPun) && (WordNo > 0))
	{
		iPunctsCount = 1;		

		if (		pWord.m_bComma 
				||	(GetOpt()->m_Language == morphGerman) // should be one clause "Staaten (5343 A)"
			) 
		{
			int ii;
			if( (ii = CanLinkSimpleSimilar(WordNo)) != -1)
			{
				iStartSearch = ii;
				return -1;
			}
        }

		// ignore full stops inside the sentence
		if( pWord.m_strWord[0] == '.') 
		{
			iStartSearch = WordNo + 1;
			return -1;				
		}

		//ignore these punctuation marks as clause borders 
		if( pWord.m_strWord.find_first_of ("%\\$#|*^")  != string::npos) 
		{
			iStartSearch = WordNo + 1;
			return -1;		
		}

		//�������� ������ "�� ������� 12,3 ����� ���.", ����� �� ���� ������� �� �����������
		if (WordNo < m_Words.size() - 1)
		{
            if( m_Words[WordNo-1].HasDes(ODigits) && m_Words[WordNo+1].HasDes(ODigits) 
				&& m_Words[WordNo].m_strWord != ")" && m_Words[WordNo].m_strWord != "(")
            {
			
				iStartSearch = WordNo + 1;
				return -1;
			}
        }

		// ���� ������������� ������������������ � �������, ���� �����, ����� �� �������, ���
		// ��� ��� ������ ������ �����  ������
        if  ( m_Words[WordNo].HasDes(OOpn) || m_Words[WordNo].HasDes(OCls) )
		{
			return  CheckIfBracketIsAClauseBorder(WordNo, iStartSearch);
        }
				
		iStartSearch = WordNo;
		//���������� ��� ������������� ����� ������� ����� ���������� � �����
		bool SearchForPuncts = true;
		int j = WordNo + 1;
		for(; j < m_Words.size() ; j++ )
		{			
            if( m_Words[j].HasDes(OPun) )
			{
				if( !SearchForPuncts )
					break;
				iPunctsCount++;
            }
			else
				if(		!m_Words[j].m_bSimilarConj
					&&	(m_Words[j].m_SubordinateConjNo == -1)
				  )
					break;
				else
					SearchForPuncts = false;			
        }
		iStartSearch = j;

		if( j >= m_Words.size() )
			return m_Words.size() - 1;

		// ���� ������ ���� ���������� ����������� ������, ����� ����� ��������  ��� � ���������� ������
		// (����� ������ ��������� ����� � ���� ������)
		if (m_Words[WordNo].HasOborot2())
		{
			iPunctsCount++;
			return WordNo;
        };
		return WordNo-1;
    };

	// ����� �� �������������� ����� 
	if  (			pWord.m_bSimilarConj 
            &&	(		m_Words[WordNo].HasDes(OEXPR1)
					||	(GetOpt()->m_Language == morphEnglish)
                 )
        )
    {
		// ����� �������� ���������� ������ FAM1-FAM2  (�� �� ����� � ��������������� �����)
		//nim : ����� �. �.
		if ( WordNo > 0 && WordNo < m_Words.size()-1 )
            if ( m_Words[WordNo-1].HasDes(OFAM1) ||
				 m_Words[WordNo+1].HasDes(OFAM2) )
			{
				iStartSearch = WordNo + 1;
				return -1;
			}
		//nim


		if( WordNo > 0 )
		{
			// �������� ��� ������������
			int ii;
			if( (ii = CanLinkSimpleSimilar(WordNo)) != -1)
			{
				iStartSearch = ii;
				return -1;
			}

			// ���� ����� ������ ����� ������ ���� ��� �������, �����  � ���� ����� ��� ���� ��������� 
			// ������� ����� �������� - ���� ���� ������.
			const CSynWord& pWord1 = m_Words[WordNo-1];	
            if	(		 pWord1.HasDes(OPun)
					||	 pWord1.m_bSimilarConj
				)
			{
				iStartSearch = WordNo + 1;
				return -1;			
			}
		}
		iStartSearch = WordNo+1;

		iPunctsCount = 0;
		return WordNo-1;
	}


	//  a multiword oborot occurs	
	if (		m_Words[WordNo].HasOborot1()
			&&	!m_Words[WordNo].HasOborot2()
		)
	{
		//  getting the end of the conjunction
		size_t W = WordNo;
		for (; W < m_Words.size(); W++)
			if (m_Words[W].HasOborot2())
				break;

		assert(W < m_Words.size());
		iStartSearch = W+1;
		//  if an oborot-conjunction terminates  the sentence
		if (iStartSearch == m_Words.size())
				return iStartSearch-1;

		// if a multiword conjunction occurs...
		// if there is no clause border just before this conjunction,i.e. there is no
		// comma and simple coord. conj., then we should  set a clause border here
		// Russian examples:
		// - �� ����� ����� ��������� ���� �� ������, ��� ������.  //�� "����" ������� �� ����� - ��� �������� ������� ������
		// - � ���� � ���, ����� �� ���� //�� "���, �����" ������� �� ����� -  ����� ��� ����� ������� ����
		// + � ���� ���, ����� �� ���� // ����� ����� ��������� ������� �� 
		if (		W < m_Words.size() 
				&&	(WordNo > 0)
				&&	!m_Words[WordNo-1].m_bSimilarConj 
                &&	!m_Words[WordNo-1].HasDes(OPun)
				&&	(m_Words[WordNo].GetOborotPtr()->HasPartOfSpeech (GetOpt()->m_Conjunction) )
			)
			return WordNo-1;
		else
		{
			return -1;
		};
	};

	//���� ��� ��������� � ����� ������, �������� ������� �� ������� ���������
	if (SetClauseBorderIfThereAreTwoPotentialPredicates(iFWrd, WordNo))
	{
		iStartSearch = WordNo+1;
		iPunctsCount = 1;
		return WordNo-1;
	}


	iStartSearch = WordNo + 1;
	return -1;


}


void CSentence::InitClauseType(CClause& clause)
{

	clause.m_vectorTypes.clear();

	vector <SClauseType> WeakTypes;

	// bHasStrongRoot is true if there is a word whose all homonyms are clause roots
	// if no such a word exists, then we should add weak clause types  to CClause::m_vectorTypes
	bool bHasStrongRoot  = false;
	for(int j = clause.m_iFirstWord; j <= clause.m_iLastWord;j++ )
	{
		EClauseType AuxVerbClauseType;
		bool bHasAuxVerb = false;

		//  pass  all auxiliary verbs
		if (!m_Words[j].m_MainVerbs.empty())
			continue;

		// if the current word has an auxiliary verb, then take the type information  from
		// the auxiliary verb, which can be used if the main verb is not predicative (for example, German participle 2) 
		int AuxVerbWordNo = FindFirstAuxVerb(j);
		if (AuxVerbWordNo != -1)  
		{
			{
				int z = FindFirstAuxVerb(AuxVerbWordNo);
				if (z != -1)
					AuxVerbWordNo = z;
			}

			bHasAuxVerb = true;
			AuxVerbClauseType = GetClauseTypeByAncodePattern(m_Words[AuxVerbWordNo].m_Homonyms[0]);
			
		};
		const CSynWord& W = m_Words[j];

		bHasStrongRoot = bHasStrongRoot || AllHomonymsArePredicates(W);
		for(int k = 0; k < W.m_Homonyms.size(); k++)
		{
			const CSynHomonym& H = W.m_Homonyms[k];
			SClauseType type;
			type.m_Type = GetClauseTypeByAncodePattern(H);
			type.m_Root.m_WordNo = j;
			type.m_Root.m_HomonymNo = k;

			if	(		GetOpt()->GetGramTab()->IsStrongClauseRoot(H.m_iPoses) 
					&&	(type.m_Type != UnknownPartOfSpeech)
				)
				clause.m_vectorTypes.push_back(type);
			else
			{
				if (		bHasAuxVerb 
						&&	(AuxVerbClauseType != UnknownPartOfSpeech)
						&&	(		(GetOpt()->m_Language != morphRussian)
								||	(type.m_Type != COMPARATIVE_T)
							)
					)
				{
					//  if the main part of an analytical verb form is not finite 
					//  then take the clause type from the auxiliary verb
					type.m_Type = AuxVerbClauseType;
					clause.m_vectorTypes.push_back(type);
				}
				else
					if (type.m_Type != UnknownPartOfSpeech)
						WeakTypes.push_back(type);
			};
		};
	};

	//  if  there is no strong roots or strong roots have homonyms then add weak roots
	if (clause.m_vectorTypes.empty() || !bHasStrongRoot) 
		clause.m_vectorTypes.insert(clause.m_vectorTypes.end(), WeakTypes.begin(),WeakTypes.end());
	
}

void CSentence::InitConjunctions(CClause* pClause)
{
	int ConjStartWordNo = pClause->m_iFirstWord;

	// passing all punctuation marks
	for(; ConjStartWordNo < m_Words.size() ; ConjStartWordNo++ )
        if( !m_Words[ConjStartWordNo].HasDes(OPun) )
			break;

	// adding all conjnctions from the beginning of the clause to m_vectorConjs
	// A subclause can contain more than on conjunction, for  example:
	//  "I know where and when it was."
	for(;;)
	{
		ConjFromWhatList FromWhere;
		int iConjIndex;
		int iWord1 = ConjStartWordNo;
		ConjStartWordNo = GetConjProperties(*this, ConjStartWordNo, FromWhere, iConjIndex,pClause->m_iLastWord);
		if( ConjStartWordNo != -1 )
		{
			if ( FromWhere == FROM_OBOR_DIC )
				pClause->m_vectorConjs.push_back(SConjIndex(FromWhere,iConjIndex,iWord1,ConjStartWordNo - 1));
			else
				pClause->m_vectorConjs.push_back(SConjIndex(FromWhere,iConjIndex,iWord1,ConjStartWordNo - 1));
		}
		else 
			break;
	}		
	// searching relative pronouns  through the whole clause
	if	(		pClause->m_vectorConjs.empty()
		)
	{
		size_t  WordNo = pClause->m_iFirstWord+1;
		if (WordNo <= pClause->m_iLastWord)
            if (m_Words[pClause->m_iFirstWord].HasDes(OPun) )
			{
				for (; WordNo <= pClause->m_iLastWord; WordNo++)
				{
					int HomonymNo;
					if (IsRelativSentencePronoun(pClause->m_iFirstWord, WordNo, HomonymNo))
					{
						pClause->m_RelativeWord.m_WordNo = WordNo;
						pClause->m_RelativeWord.m_HomonymNo = HomonymNo;
						

						return ;
					}
				};
			};

	}
	// if no conjunction at the beginning of the clause was found, and 
	//  relative pronoun, then go through the whole  clause and look for a conjunction
	if( pClause->m_vectorConjs.empty() )
	{
		for(int i = pClause->m_iFirstWord +1 ; i < pClause->m_iLastWord; i++ )
			if	(m_Words[i].m_SubordinateConjNo != -1 ) 
			{
				pClause->m_vectorConjs.push_back(SConjIndex(FROM_SUB_CONJ,m_Words[i].m_SubordinateConjNo,i,i));
				break;
			}
	};


}


bool CSentence::BuildInitialClauses()
{
    
	CClauseCollection::Clear();
    IsValid();

	m_bFirstInPairFound = false;

	
	int iFirstWord, iLastWord;
	iFirstWord = 0;	
	
	int iStartSearch;
	int iPunctSignsCount = 0;	

	

	for(int WordNo = 0 ; WordNo < m_Words.size();  WordNo = iStartSearch)
	{		
		int iNextPunctCount = 0;
		if	(		((iLastWord = IsClauseBorder(WordNo, iStartSearch,iNextPunctCount, iFirstWord)) != -1) 
				&&	(iLastWord >= iFirstWord)
			)
		{
			if( m_pSyntaxOptions->m_KillHomonymsMode != DontKillHomonyms )
				SolveAmbiguityUsingRuleForTwoPredicates(iFirstWord,iLastWord);

			//creating clause			
			CClause clause(this, iFirstWord,iLastWord);
			//assigning clause type
			InitClauseType(clause);
			int debug = clause.m_vectorTypes.size();
			clause.m_iPunctSignsCount = iPunctSignsCount;
			iPunctSignsCount = iNextPunctCount;
			InitConjunctions(&clause);			
			AddClause(clause);
			iFirstWord = iLastWord + 1;	
		} 

		
	}


	return true;
}



