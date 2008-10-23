#include "../AgramtabLib/agramtab_.h"
#include "AncodePattern.h"



CAncodePattern::CAncodePattern(const	CAgramtab* pGramTab): m_pGramTab(pGramTab)
{
    ResetFlags();
    m_LemSign = 0;
};

const	CAgramtab* CAncodePattern::GetGramTab() const
{
    return m_pGramTab;
}


void CAncodePattern::ResetFlags()
{
    // do not reset m_GramCodes, m_CommonGramCode and m_LemSign - this is the  source  information
	m_iGrammems = 0;
	m_iPoses = 0;
	m_TypeGrammems = 0;
    m_bUnkGramcodes = false;

}

void CAncodePattern::CopyAncodePattern(const CAncodePattern& X)
{
	m_iGrammems = X.m_iGrammems;
	m_iPoses = X.m_iPoses;
	m_GramCodes = X.m_GramCodes;
	m_SimplePrepNos = X.m_SimplePrepNos;
	m_LemSign = X.m_LemSign;
	m_CommonGramCode = X.m_CommonGramCode;
	m_TypeGrammems = X.m_TypeGrammems;
    m_pGramTab = X.m_pGramTab;
};


bool CAncodePattern::HasGrammem(BYTE gram) const 
{ 
	return		((m_iGrammems    & _QM(gram)) > 0)
			||	((m_TypeGrammems & _QM(gram)) > 0);
}

bool CAncodePattern::HasPos(BYTE pos) const
{ 
	return (m_iPoses & (1 << pos)) > 0; 
};


string CAncodePattern::GetGrammemsByAncodes() const
{
	string Result;
	for (int i = 0; i < m_GramCodes.length(); i += 2)
	{
		QWORD g;
        if (!GetGramTab()->GetGrammems(m_GramCodes.c_str()+i, g))
		{
				assert (false);
		};
		Result += GetGramTab()->GrammemsToStr(g);
		Result += "; ";

	};
	return Result;
};


bool CAncodePattern::DeleteAncodesByGrammemIfCan(BYTE Grammem)
{
    string GramCodes;
    for (int j=0; j < m_GramCodes.length(); j+=2) 
	    if (m_GramCodes[j] != '?')
	    {
		    QWORD Grammems;
		    GetGramTab()->GetGrammems ( m_GramCodes.c_str() + j, Grammems);
		    if (   (Grammems & _QM(Grammem)) == 0 )
			      GramCodes += string(m_GramCodes.c_str() + j, 2);
	    }
    if (GramCodes.empty())
        return false;
    m_GramCodes = GramCodes;
    InitAncodePattern();
    return true;
}

bool CAncodePattern::ModifyGrammems(QWORD Grammems , size_t Poses)
{
	string strOldGramcodes = m_GramCodes;	
	QWORD  savegrammems = m_iGrammems;	
	size_t saveposes = m_iPoses;
	m_iGrammems = 0;
	m_iPoses = 0;
	m_GramCodes = "";
	
    if (strOldGramcodes.empty() || strOldGramcodes[0] == '?') 
    {
        int uu =0;;
    }
    else
	for (size_t j=0; j < strOldGramcodes.length(); j+=2)
	{
			QWORD CurrGrammems = 0;		
			bool b = GetGramTab()->GetGrammems(strOldGramcodes.c_str() + j, CurrGrammems);
			assert (b);
			if (!b)
			{
                ErrorMessage(Format("Cannot get grammems by gramcode %s ",strOldGramcodes.substr(j,2).c_str()));
				m_iGrammems = 0;
				break;
			};
			BYTE CurrPOS = GetGramTab()->GetPartOfSpeech(strOldGramcodes.c_str() + j);
			
			/*
				there are tow possibilities:
					1. "Grammems" contains only one or two grammems, and all gramcodes of this 
						pattern should contain this "Grammems"
					2. "Grammems" is a union of all possible grammems and all gramcodes should be inside
					   this union.

			*/
			if (Poses & (1<<CurrPOS))
				if(    ((Grammems & CurrGrammems) == CurrGrammems) 
					|| ((Grammems & CurrGrammems) == Grammems)
				)
				{
					m_iGrammems |= CurrGrammems;
					m_GramCodes += strOldGramcodes.substr(j,2);
                    BYTE pos = GetGramTab()->GetPartOfSpeech(strOldGramcodes.c_str() + j);
					m_iPoses |= (1 << pos);

				}
			
	}		
	if ( (m_iGrammems == 0) && (savegrammems != 0))
	{
		m_GramCodes = strOldGramcodes;	
		m_iGrammems = savegrammems;	
		m_iPoses = saveposes;
		return false;
	}
	//assert (!m_GramCodes.empty());
	return true;
};


bool CAncodePattern::InitAncodePattern()
{
    ResetFlags();
    m_bUnkGramcodes = m_GramCodes.empty() || ((unsigned char)m_GramCodes[0] == '?');	
    

	if	(		!m_GramCodes.empty()
			&& 	(m_GramCodes[0] != '?') 
		)
	{
		for (size_t j=0; j < m_GramCodes.length(); j+=2)
		{
				QWORD CurrGrammems = 0;		
				bool b = GetGramTab()->GetGrammems(m_GramCodes.c_str() + j, CurrGrammems);
				assert (b);
				if (!b)
				{
                    ErrorMessage(Format("Cannot get grammems by gramcode %s ",m_GramCodes.substr(j,2).c_str()));
				};
				m_iGrammems |= CurrGrammems;
                BYTE pos = GetGramTab()->GetPartOfSpeech(m_GramCodes.c_str() + j);
				m_iPoses |= (1 << pos);
				
		}		
	}
	
	if	(		(m_CommonGramCode.length() == 2)
			&&	(m_CommonGramCode != "??")
		)
	{
		bool b = GetGramTab()->GetGrammems(m_CommonGramCode.c_str(), m_TypeGrammems);
		assert (b);
		if (!b)
		{
			ErrorMessage(Format("Cannot get grammems by type gramcode %s ",m_CommonGramCode.c_str()));
		};
	};


	return true;
};

void  CAncodePattern::SetMorphUnknown()
{
	m_CommonGramCode = "??";
	m_LemSign = '-';
    m_GramCodes = "??";
    m_bUnkGramcodes = true;
    InitAncodePattern();
};

string	CAncodePattern::GetPartOfSpeechStr() const
{
	for (BYTE i=0; i< 32; i++)  
        if (HasPos(i))
            return GetGramTab()->GetPartOfSpeechStr(i);
    return "";
};
