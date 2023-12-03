#include "LemWord.h"
#include "gra_descr.h"
#include "morph_dict/agramtab/RusGramTab.h"
#include "morph_dict/agramtab/agramtab_.h"


CLemWord::CLemWord() 
{
	Reset();	
};



#define   StupidSymbol1 160
extern int GetMorphSignPosition(const char* LineStr);


void CLemWord::DelDes(Descriptors g) 
{
    m_GraDescrs &= ~ ( _QM(g) );
}


void CLemWord::AddDes(Descriptors g) 
{
    m_GraDescrs |= ( _QM(g) );
}


bool CLemWord::HasDes(Descriptors g) const
{
    return (m_GraDescrs & _QM(g)) == _QM(g);
}

// returns the end point of the graphematical descriptors
void CLemWord::ProcessGraphematicalDescriptors(const std::string& s)
{

    m_GraDescrs = parse_gra_descriptors(s.c_str(), m_UnparsedGraphemDescriptorsStr);

	m_bSpace =			HasDes(OSpc) 
					||	HasDes(OEOLN) 
					||	((BYTE)m_strWord[0] == StupidSymbol1)
					||	( ((BYTE)m_strWord[0] == '_') && (m_strWord.length() == 1));

    

	if (HasDes (OUpLw))
		m_Register = UpLow;
	else
	if (HasDes (OUp))
		m_Register = UpUp;
	else
	if (HasDes (OLw))
		m_Register = LowLow;		
	else
		m_Register = AnyRegister;		

	m_bComma = (m_strWord.length() == 1)  && (m_strWord[0] == ',');	
	m_bDash = (m_strWord.length() == 1)  && (m_strWord[0] == '-');	

	bool bRomanNumber = is_roman_number(m_strWord.c_str(), m_strWord.length() );
	size_t hyphen_occur = m_strWord.find("-");
	if ((hyphen_occur != std::string::npos) && (hyphen_occur!=0))
	{
		// "Павла I-го" 
		// "I-го" - одно слово
		bRomanNumber = is_roman_number(m_strWord.c_str(), hyphen_occur);
	};
	if (bRomanNumber)
        m_GraDescrs |= _QM(ORoman);

    m_bWord  = !bRomanNumber && (HasDes(ORLE) || HasDes(OLLE)); 

	
}

void rtrim (char* s, size_t* len)
 {
     // откусываю признаки конца строки, если они появились
   while (*len > 0 && isspace((unsigned char)s[*len-1]))
	s[--(*len)] = 0;
 }


bool CLemWord::AddNextHomonym(const std::string& strPlmLine)
{
	auto columns = split_string(strPlmLine, '\t');
	assert(columns.size() == 4);

    assert ( GetHomonymsCount() > 0);

	CHomonym* pHomonym = AddNewHomonym();

	ProcessGraphematicalDescriptors(columns[2]);

	if (!pHomonym->ProcessLemmaAndGrammems(columns[3])) return false;

    {
        const CHomonym* pFirstHom = GetHomonym(0);
        pHomonym->m_bInOb = pFirstHom->m_bInOb;
        pHomonym->m_bOborot1 = pFirstHom->m_bOborot1;
        pHomonym->m_bOborot2 = pFirstHom->m_bOborot2;
        pHomonym->m_OborotNo = pFirstHom->m_OborotNo;
    }

	InitLevelSpecific(pHomonym);

	return true;

}


int ParseOborotNo(const std::string& Descriptors)
{
	const char* strObor = strstr(Descriptors.c_str(),"EXPR_NO");
	if( strObor )
		return atoi (strObor+7);
    else
        return -1;
};


bool CLemWord::ProcessPlmLineForTheFirstHomonym(std::string sPlmLine, int& OborotNo)
{
    // откусываю признаки конца строки, если они появились
	Trim(sPlmLine);
	auto columns = split_string(sPlmLine, '\t');
	assert(columns.size() == 3 || columns.size() == 4);
    SetWordStr(columns[0]);

	//  reading file position of an item from graline
    if (sscanf(columns[1].c_str(), "%i %i", &m_GraphematicalUnitOffset, &m_TokenLengthInFile) != 2)
        return false;

    ProcessGraphematicalDescriptors(columns[2]);

    if( m_bSpace )
        return true;

    CHomonym*  pHomonym  = AddNewHomonym();

    if (HasDes(OEXPR1))
        OborotNo = ParseOborotNo(m_UnparsedGraphemDescriptorsStr);

    if (OborotNo != -1)
    {
        pHomonym->m_OborotNo =  OborotNo;
        pHomonym->m_bInOb =  true;
        pHomonym->m_bOborot1 = HasDes(OEXPR1);
        pHomonym->m_bOborot2 = HasDes(OEXPR2);
    }
    if (HasDes(OEXPR2))
        OborotNo = -1;

	if (columns.size() == 3)
        pHomonym->SetLemma(m_strWord);
    else
        if (!pHomonym->ProcessLemmaAndGrammems(columns[3]))
            return false;

    InitLevelSpecific(pHomonym);

    if	(		(m_strWord == "\"")
        ||	(m_strWord == "'")
        )
        DelDes(OPun);

    if (pHomonym->m_LemSign != '+')
        m_bPredicted = true;

    return true;
}




void CLemWord::SetAllOtherHomsDel(int iHom)
{
    for(int i = 0 ; i < GetHomonymsCount() ; i++ )
	{
		GetHomonym(i)->m_bDelete =  (i != iHom);
	}
}


bool CLemWord::IsFirstOfGraPair() const
{
	return		
				HasDes(OFile1)
            ||	HasDes(OGerDivComp1)
			||	HasDes(OEXPR1) 
			||	HasDes(OKey1)
			||	HasDes(OFAM1);
}

bool CLemWord::IsSecondOfGraPair() const
{
	return		
				HasDes(OFile2)
			||	HasDes(OGerDivComp2)
			||	HasDes(OEXPR2)
			||	HasDes(OKey2)
			||	HasDes(OFAM2);
}

bool CLemWord::IsFirstOfGraPair(EGraPairType type) const
{

	switch(type)
	{

		case WebAddr:
			return HasDes(OFile1);
	
		case Oborot:
			return HasDes(OEXPR1) ;	

		case Keyb:
			return HasDes(OKey1);

		case GermanDividedCompound:
			return HasDes(OGerDivComp1);

	
	}
	return false;
}


bool CLemWord::IsSecondOfGraPair(EGraPairType type) const
{
	switch(type)
	{

		case WebAddr:
			return HasDes(OFile2);
	
		case Oborot:
			return HasDes(OEXPR2) ;	

		case Keyb:
			return HasDes(OKey2);

		case GermanDividedCompound:
			return HasDes(OGerDivComp2);
	
	}
	return false;

}

void CLemWord::Reset()
{
    m_GraDescrs = 0;
	m_bSpace = false;
	m_bWord = false; 
	m_bDash = false;
	m_Register = AnyRegister;
	m_bComma = false;
	m_bPredicted = false;
	m_GraphematicalUnitOffset = -1;
    m_TokenLengthInFile = 0;
	m_bHasSpaceBefore = false;
	m_bDeleted = false;
	m_LettersCount = 0;
}





void CLemWord::DeleteOborotMarks()
{
    DelDes(OEXPR1);
    DelDes(OEXPR2);

    size_t i = m_UnparsedGraphemDescriptorsStr.find("EXPR_NO");
    if (i != std::string::npos)
	{
		m_UnparsedGraphemDescriptorsStr.erase(i,strlen ("EXPR_NO"));

		while	(			(i < m_UnparsedGraphemDescriptorsStr.length())
					&&		(isdigit((unsigned char)m_UnparsedGraphemDescriptorsStr[i]))
				)
		m_UnparsedGraphemDescriptorsStr.erase(i, 1);
	};

	for(int i = 0 ; i < GetHomonymsCount() ; i++ )
		GetHomonym(i)->DeleteOborotMarks();
}

int		CLemWord::GetOborotNo() const
{
    return GetHomonym(0)->m_OborotNo;
};


bool		CLemWord::HasOborot1() const
{
    for(int i = 0 ; i < GetHomonymsCount() ; i++ )
        if ( GetHomonym(i)->m_bOborot1 )
            return true;;
    return false;
};

bool		CLemWord::HasOborot2() const
{
    for(int i = 0 ; i < GetHomonymsCount() ; i++ )
        if ( GetHomonym(i)->m_bOborot2 )
            return true;;
    return false;
};



bool		CLemWord::IsInOborot() const
{
    for(int i = 0 ; i < GetHomonymsCount() ; i++ )
        if ( GetHomonym(i)->m_bInOb )
            return true;;
    return false;
	
};


bool  CLemWord::CanBeSynNoun() const
{
	for(int i = 0 ; i < GetHomonymsCount() ; i++ )
		if (GetHomonym(i)->IsSynNoun() )
			return true;	

	return false;
}




void CLemWord::KillHomonymOfPartOfSpeech(int iPartOfSpeech)
{
    if (GetHomonymsCount() <= 1)
		return;

	for(int i = 0 ; i < GetHomonymsCount(); i++)
	{
        if( GetHomonym(i)->HasPos( iPartOfSpeech ) )
		{
			EraseHomonym( i );
			break;
		}
	}
}
void CLemWord::SetHomonymsDel(bool Value)
{
	for(int i = 0 ; i < GetHomonymsCount() ; i++ )
	{
		GetHomonym(i)->m_bDelete = Value;
	}
}


void CLemWord::DeleteMarkedHomonymsBeforeClauses()
{
    for(int i = (int)GetHomonymsCount()- 1 ; i >= 0 ; i-- )
	{
		if( GetHomonym(i)->m_bDelete )
			EraseHomonym(i);			
	}

}

void	CLemWord::SetWordStr (std::string NewValue)
{
	m_strWord = NewValue;
	m_strUpperWord =  NewValue;
	MakeUpperUtf8(m_strUpperWord);
	m_LettersCount = CountLettersInUtf8(NewValue);
	
};



bool CLemWord::FindLemma(std::string strLemma)  const
{

	for(int i = 0 ; i < GetHomonymsCount(); i++)
		if (GetHomonym(i)->IsLemma(strLemma.c_str())) 
			return true;

	return false;	
}

int  CLemWord::GetHomonymByPOS(BYTE POS) const
{
	for(int i = 0 ; i < GetHomonymsCount(); i++)
		if( GetHomonym(i)->HasPos(POS) )
			return i;	

	return -1;
}

part_of_speech_mask_t  CLemWord::GetPoses() const
{
    part_of_speech_mask_t Poses = 0;
	for(int i = 0 ; i < GetHomonymsCount(); i++)
        Poses |= GetHomonym(i)->m_iPoses;
	return Poses;
}

uint64_t CLemWord::GetGrammems() const
{
    uint64_t G = 0;
	for(int i = 0 ; i < GetHomonymsCount(); i++)
        G |= GetHomonym(i)->m_iGrammems;
	return G;
}

bool  CLemWord::HasPos(BYTE POS) const
{
    return GetHomonymByPOS(POS) != -1;
}

bool    CLemWord::HasGrammem(BYTE Grammem) const
{
    return GetHomonymByGrammem(Grammem) != -1;
}

int CLemWord::GetHomonymByGrammem(BYTE grammem) const
{
	for(int i = 0 ; i < GetHomonymsCount(); i++)
		if( GetHomonym(i)->HasGrammem(grammem) )
			return i;	

	return -1;
}

int CLemWord::GetHomonymByPOSandGrammem(BYTE POS, BYTE grammem) const
{
	for(int i = 0 ; i < GetHomonymsCount(); i++)
		if(			GetHomonym(i)->HasGrammem(grammem) 
				&&	GetHomonym(i)->HasPos(POS)
			)
			return i;	

	return -1;
}

int CLemWord::GetHomonymByPosesandGrammem(part_of_speech_mask_t Poses, BYTE grammem) const
{
	for(int i = 0 ; i < GetHomonymsCount(); i++)
		if(			GetHomonym(i)->HasGrammem(grammem) 
				&&	(GetHomonym(i)->m_iPoses & Poses)
			)
			return i;	

	return -1;
}





std::string CLemWord :: BuildGraphemDescr ()  const
{
    std::string Result;
    for (int l=0;l<63;l++)     // write descriptors 
        if ( HasDes(((Descriptors)l) )) 
		{  
			Result += " ";
			Result +=  GetDescriptorStr(l);
		};
    Result += " " +m_UnparsedGraphemDescriptorsStr;
    return Result;
}


std::string CLemWord::GetDebugString(const CHomonym* pHomonym, bool bFirstHomonym)  const
{
    std::string Result;
    if (!bFirstHomonym) Result = "  ";
	if(!pHomonym) pHomonym = GetHomonym(0);
	Result += m_strWord;
	Result += " ";
    Result += Format (" %i %i ", m_GraphematicalUnitOffset, m_TokenLengthInFile);
    Result += BuildGraphemDescr();
	if (pHomonym->m_LemSign != 0)
	{
        assert (!pHomonym->GetLemma().empty());
        assert (!pHomonym->GetGramCodes().empty());
        assert (!pHomonym->m_CommonGramCode.empty());

        Result += " " + Format("%c",pHomonym->m_LemSign);
        Result += " " + pHomonym->GetLemma();
        Result += " " + pHomonym->GetGramTab()->GetTabStringByGramCode(pHomonym->m_CommonGramCode.c_str());
        for (int i=0; i < pHomonym->GetGramCodes().length(); i+=2)
            Result += " " + pHomonym->GetGramTab()->GetTabStringByGramCode(pHomonym->GetGramCodes().c_str()+i);
    }
	return  Result;
}

std::string CLemWord :: GetPlmStr (const CHomonym* pHomonym, bool bFirstHomonym)  const
{

    std::string Result;

    if (!bFirstHomonym) Result = "  ";

	Result += m_strWord;
	Result += "\t";
    Result += Format ("%i %i\t", m_GraphematicalUnitOffset, m_TokenLengthInFile);
    Result += BuildGraphemDescr();

	if (pHomonym->m_LemSign != 0)
	{
        assert (!pHomonym->GetLemma().empty());
        assert (!pHomonym->GetGramCodes().empty());
        assert (!pHomonym->m_CommonGramCode.empty());
        
        Result += std::string("\t") + pHomonym->m_LemSign + pHomonym->m_CommonGramCode + " " + pHomonym->GetLemma() + " " + pHomonym->GetGramCodes() + " ";
        Result +=  Format("%i %i", pHomonym->m_lPradigmID, pHomonym->m_lFreqHom);
	};

	return  Result;
};


bool CLemWord::HasAnalyticalBeRus() const
{
	// если мы попали на оборот(например, "может быть"), тогда не будем строить здесь анал. форму.
	if (IsInOborot()) return false;

	// "быто" предсказывается как "быть"
	if (m_bPredicted) return false;

    for (int i = 0; i < GetHomonymsCount(); i++)
		if	(		(GetHomonym(i)->HasPos(VERB) || GetHomonym(i)->HasPos(INFINITIVE)) 
				&& 	( GetHomonym(i)->IsLemma("БЫТЬ") || GetHomonym(i)->IsLemma("СТАТЬ"))
			)
			return true;
		
	return false; 	
}
