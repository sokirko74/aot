#include "LemWord.h"
#include "common/gra_descr.h"
#include "morph_dict/agramtab/RusGramTab.h"
#include "morph_dict/agramtab/GerGramTab.h"
#include "morph_dict/agramtab/agramtab.h"
#include "morph_dict/lemmatizer_base_lib/Lemmatizers.h"


CLemWord::CLemWord(MorphLanguageEnum l)
{
	m_Language = l;
	Reset();	
};

void CLemWord::Reset()
{
	m_bFirstUpperAlpha = false;
	m_bQuoteMark = false;
	m_GraDescrs = 0;
	m_bSpace = false;
	m_bWord = false;
	m_bDash = false;
	m_Register = AnyRegister;
	m_bComma = false;
	m_GraphematicalUnitOffset = -1;
	m_bHasSpaceBefore = false;
	m_bDeleted = false;
	m_LettersCount = 0;
	m_TokenType = OTHER_TOKEN_TYPE;
}

//void CLemWord(const CLemWord& c)
//{
//
//}


#define   StupidSymbol1 160


void CLemWord::DelDes(Descriptors g) 
{
    m_GraDescrs &= ~ ( _QM(g) );
}


void CLemWord::AddDes(Descriptors g) 
{
    m_GraDescrs |= ( _QM(g) );
}

size_t	CLemWord::GetHomonymsCount() const
{
	return m_MorphHomonyms.size();
}

const CHomonym* CLemWord::GetHomonym(int i) const
{
	return &m_MorphHomonyms[i];
}

CHomonym* CLemWord::GetHomonym(int i)
{
	return &m_MorphHomonyms[i];
}

void CLemWord::EraseHomonym(int iHom)
{
	m_MorphHomonyms.erase(m_MorphHomonyms.begin() + iHom);
}

CHomonym* CLemWord::AddNewHomonym()
{
	CHomonym hom(m_Language);
	m_MorphHomonyms.push_back(hom);
	return &m_MorphHomonyms.back();
}


const std::string& CLemWord::GetWord() const
{
	return m_strWord;
};

void CLemWord::CreateDefaultHomonym(short oborot_no)
{
	if (GetHomonymsCount() > 0 || m_bSpace) {
		return;
	}
	CHomonym* h = AddNewHomonym();
	if (!HasDes(OPun)) {
		h->SetPredictedWord("??");
	}
	h->SetLemma(m_strUpperWord);
	InitLevelSpecific(oborot_no, h);
}

void CLemWord::InitLevelSpecific(short oborot_no, CHomonym* pHom)
{
	m_bFirstUpperAlpha = (m_Register == UpUp) || (m_Register == UpLow);
	m_bQuoteMark = HasDes(OPun) && m_strWord.find("\"") != std::string::npos;
	pHom->InitAncodePattern();

	if (oborot_no != -1)
	{
		pHom->m_OborotNo = oborot_no;
		pHom->m_bInOb = true;
		pHom->m_bOborot1 = HasDes(OEXPR1);
		pHom->m_bOborot2 = HasDes(OEXPR2);
	}
	if ((m_strWord == "\"") || (m_strWord == "'"))
		DelDes(OPun);

}

void CLemWord::DeleteAllHomonyms()
{
	m_MorphHomonyms.clear();
}

void CLemWord::SafeDeleteMarkedHomonyms()
{
	for (int i = (int)GetHomonymsCount() - 1; i >= 0; i--)
	{
		if (GetHomonymsCount() == 1) return;
		if (GetHomonym(i)->m_bDelete)
			EraseHomonym(i);
	}
}

void CLemWord::CreateFromToken(const CGraLine& token) {
	m_GraDescrs = token.GetDescriptors();
	m_strWord = token.GetTokenUtf8();
	m_strUpperWord = token.GetTokenUpperUtf8();
	m_LettersCount = token.GetLettersCount();
	m_GraphematicalUnitOffset = token.GetInputOffset();
	ProcessGraphematicalDescriptors();
}

bool CLemWord::LemmatizeFormUtf8(const std::string& s, const CLemmatizer* pLemmatizer)
{
	std::vector<CFormInfo> Paradigms;
	std::string word_s8 = convert_from_utf8(s.c_str(), pLemmatizer->GetLanguage());
	if (!pLemmatizer->CreateParadigmCollection(false, word_s8, false, false, Paradigms)) return false;
	if (Paradigms.empty()) return false;
	DeleteAllHomonyms();
	for (size_t i = 0; i < Paradigms.size(); i++)
	{
		CHomonym H(m_Language);
		H.SetHomonym(&Paradigms[i]);
		m_MorphHomonyms.push_back(H);
	}
	return true;

};

bool CLemWord::HasDes(Descriptors g) const
{
    return (m_GraDescrs & _QM(g)) == _QM(g);
}



void CLemWord::ProcessGraphematicalDescriptors()
{
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
    m_bWord  = !HasDes(ORoman) && (HasDes(ORLE) || HasDes(OLLE)); 

	if (HasDes(ORLE))
		m_TokenType = ORLE;
	else if (HasDes(OLLE)) 
		m_TokenType = OLLE;
	else if (HasDes(ODigits))
		m_TokenType = ODigits;
	else if (HasDes(ONumChar))
		m_TokenType = ONumChar;
	else if (HasDes(OPun))
		m_TokenType = OPun;
	else 
		m_TokenType = (Descriptors)OTHER_TOKEN_TYPE;
}


int ParseOborotNo(const std::string& Descriptors)
{
	const char* strObor = strstr(Descriptors.c_str(),"EXPR_NO");
	if( strObor )
		return atoi (strObor+7);
    else
        return -1;
};






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


bool CLemWord::IsPredicted() const {
	return !m_MorphHomonyms.empty() && m_MorphHomonyms[0].m_SearchStatus == PredictedWord;
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
	std::string result = " " + CGraLine::GetStringByDescriptors(m_GraDescrs);
	if (HasDes(OEXPR1) && !m_MorphHomonyms.empty()) {
		result += Format(" EXPR_NO%i", m_MorphHomonyms[0].m_OborotNo);
	}
	result += " " + m_UnparsedGraphemDescriptorsStr;
    return result;
}


std::string CLemWord::GetDebugString(const CHomonym* pHomonym, bool bFirstHomonym)  const
{
    std::string Result;
    if (!bFirstHomonym) Result = "  ";
	if (!pHomonym) pHomonym = GetHomonym(0);
	Result += m_strWord;
	Result += " ";
    Result += Format (" %i %i ", m_GraphematicalUnitOffset, m_LettersCount);
    Result += BuildGraphemDescr();
	if (pHomonym->m_SearchStatus != NotWord)
	{
		Result += pHomonym->GetDebugString();
    }
	return  Result;
}

bool CLemWord::HasAnalyticalBeRus() const
{
	// если мы попали на оборот(например, "может быть"), тогда не будем строить здесь анал. форму.
	if (IsInOborot()) return false;

	for (int i = 0; i < GetHomonymsCount(); i++) {
		auto h = GetHomonym(i);
		if (   h->m_SearchStatus == DictionaryWord // 	// "быто" предсказывается как "быть"
			&& (h->HasPos(VERB) || h->HasPos(INFINITIVE))
			&& (h->IsLemma("БЫТЬ") || h->IsLemma("СТАТЬ"))
			)
			return true;
	}
		
	return false; 	
}

void CLemWord::AddDescriptor(const std::string& some_string_label)
{
	for (auto& h: m_MorphHomonyms)
	{
		h.m_OtherWordDescriptors += " " + some_string_label;
	};
};

void CLemWord::DeleteHomonymByNotTerminalSymbol(const CInputSymbol& s)
{
	for (int i = (int)m_MorphHomonyms.size() - 1; i >= 0; i--)
	{
		auto& all = m_MorphHomonyms[i].m_AutomatSymbolInterpetation;
		if (all.find(s) == all.end()) {
			m_MorphHomonyms.erase(m_MorphHomonyms.begin() + i);
		}
	};

};


bool CheckGrammems(const CLemWord& w, const CHomonym& h, const CGrammarItem& I)
{

	if (I.m_MorphPattern.m_SearchStatus == PredictedWord) {
		if (h.m_lPradigmID != UnknownParadigmId) return false;
	}

	//  check pos
	if ((h.m_iPoses & I.m_MorphPattern.m_iPoses) != I.m_MorphPattern.m_iPoses)
		return false;

	if ((I.m_MorphPattern.m_iGrammems & h.m_iGrammems) == I.m_MorphPattern.m_iGrammems)
		return true;

	// German genitive ad hoc rule
	if ((I.m_MorphPattern.m_iGrammems == _QM(gGenitiv))
		&& h.m_lPradigmID == UnknownParadigmId
		&& endswith(w.m_strUpperWord, "S")
		)
		return true;

	return false;

};

bool CLemWord::IsEqualToGrammarItem(const CHomonym& h, const CGrammarItem& I) const
{
	if (I.m_TokenType != OTHER_TOKEN_TYPE) {
		if (I.m_TokenType != m_TokenType) {
			return false;
		}
	}

	if (!I.m_MorphPattern.HasNoInfo()) {
		if (m_TokenType != ORLE && m_TokenType != OLLE)
			return false;
		if (!CheckGrammems(*this, h, I))
			return false;
	}


	if (!I.m_Token.empty()) {
		auto equal_to_lemma = h.GetLemma() == I.m_Token;
		// or equality with right truncation
		auto equal_to_lemma_suffix = (I.m_Token[0] == '*') && endswith(h.GetLemma(), I.m_Token.substr(1));
		if (!equal_to_lemma && !equal_to_lemma_suffix)
			return false;
	}

	if (I.m_pListFile != 0)
	{
		const StringSet& lemmas = I.m_pListFile->m_PossibleLemmas;
		bool found = lemmas.find(h.GetLemma()) != lemmas.end();
		if (!found && h.m_lPradigmID == UnknownParadigmId) {
			found = lemmas.find(m_strUpperWord) != lemmas.end();
		}
		if (!found)
		{
			size_t hyphen = h.GetLemma().rfind("-"); // and if we cannot find the token substd::string  after the last hyphen
			found = (hyphen != std::string::npos) && (lemmas.find(h.GetLemma().substr(hyphen + 1)) != lemmas.end());
		};
		if (!found) {
			return false;
		}
	}


	return true;
};

void CLemWord::BuildTerminalSymbolsByWord(const std::vector<CGrammarItem>& grm_items, size_t end_of_stream_symbol)
{
	assert(GetHomonymsCount() > 0);
	m_AutomatSymbolInterpetationUnion.clear();
	for (size_t i = 0; i < GetHomonymsCount(); ++i) {
		GetHomonym(i)->m_AutomatSymbolInterpetation.clear();
	}

	if (m_bDeleted)
		return;

	// adding an end of stream symbol to each word
	m_AutomatSymbolInterpetationUnion.insert(CInputSymbol(end_of_stream_symbol, "", ""));

	
	for (size_t i = 0; i < grm_items.size(); ++i)
	{
		auto& item = grm_items[i];
		if (item.m_bMeta) continue;

		if (!item.m_bCanHaveManyHomonyms && (m_MorphHomonyms.size() > 1)) continue;

		if (item.m_Register != AnyRegister)
		{
			if (item.m_Register != m_Register)
				continue;
		};

		if ((m_GraDescrs & item.m_GraphemDescrs) != item.m_GraphemDescrs) {
			continue;
		}

		for (size_t k = 0; k < GetHomonymsCount(); ++k) {
			auto& h = *GetHomonym(k);
			if (IsEqualToGrammarItem(h, item))
			{
				CInputSymbol N(i, h.GetGramCodes(), h.m_CommonGramCode);
				h.m_AutomatSymbolInterpetation.insert(N);
				m_AutomatSymbolInterpetationUnion.insert(N);
			};
		}
	}

};