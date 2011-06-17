#include "SimpleGrammarLib.h"
#include "GrammarItem.h"
#include "../common/gra_descr_primitives.h"
extern string CurrentSourceFileName;


//==========================================
void	CMorphPattern::CopyFrom(const CMorphPattern& P)
{
	m_Grammems = P.m_Grammems;
	m_Poses = P.m_Poses;
	m_GrmAttribute = P.m_GrmAttribute;
	m_SearchStatus = P.m_SearchStatus;
};

void CMorphPattern::SetNull()
{
	m_Grammems = 0;
	m_Poses = 0;
	m_GrmAttribute = "";
	m_SearchStatus = AnyStatus;
};

bool CMorphPattern::operator < (const CMorphPattern& _X1) const
{
	if (m_Grammems != _X1.m_Grammems)
		return m_Grammems < _X1.m_Grammems;

	if (m_Poses != _X1.m_Poses)
		return m_Poses < _X1.m_Poses;

	return m_SearchStatus < _X1.m_SearchStatus;
};


bool CMorphPattern::operator ==(const CMorphPattern& _X1) const
{
	return		m_Grammems == _X1.m_Grammems
			&&	m_Poses == _X1.m_Poses
			&&	m_SearchStatus == _X1.m_SearchStatus;
};

bool  CMorphPattern::Init(const CAgramtab* GramTab, string& ErrorMsg)
{
	Trim (m_GrmAttribute);
	if (m_GrmAttribute.empty())	return true;
	string s = m_GrmAttribute;
	if (m_GrmAttribute[0] == '-')
	{
		m_SearchStatus = NotFoundInDictionary;
		s.erase(0,1);
	}
	else
	if (m_GrmAttribute[0] == '+')
	{
		m_SearchStatus = FoundInDictionary;
		s.erase(0,1);
	}
	Trim (s);
	BYTE PartOfSpeech;
	if (GramTab)
		if (		!GramTab->ProcessPOSAndGrammems(s.c_str(), PartOfSpeech, m_Grammems)
				&&	!GramTab->ProcessPOSAndGrammems(string("* "+s).c_str(), PartOfSpeech, m_Grammems)
			)
		{
			ErrorMsg = "Bad morphologiucal description " + m_GrmAttribute;
			return false;
		};

	if (PartOfSpeech != UnknownPartOfSpeech)
		m_Poses = 1 << PartOfSpeech;
	else
		m_Poses = 0;

	return true;
};


string  CMorphPattern::ToString() const
{
	string Result;
	#ifdef WIN32
		Result = Format("%I64i", m_Grammems);
	#else
		Result = Format("%lli", m_Grammems);
	#endif

	Result += Format(" %i %i %s\x1\n", (int)m_Poses, (int)m_SearchStatus, m_GrmAttribute.empty() ? "null" : m_GrmAttribute.c_str() );
	return Result;

};

bool CMorphPattern::FromString(const string& line)
{
	char buff1[1024];
	int iStatus;
	#ifdef WIN32
		if (sscanf(line.c_str(),  "%I64i %i %i %[^\x1]", &m_Grammems, &m_Poses, &iStatus,buff1) != 4) return false;
	#else
		if (sscanf(line.c_str(),  "%lli %i %i %[^\x1]", &m_Grammems,&m_Poses, &iStatus, buff1) != 4) return false;
	#endif

	m_GrmAttribute = buff1;
	if (m_GrmAttribute == "null")
		m_GrmAttribute = "";
	m_SearchStatus = (MorphSearchStatus)iStatus;
	return true;
};

//==========================================

void CGrammarItem::InitGrammarItem()
{
	m_bGrammarRoot = true;
	m_bCanHaveManyHomonyms = true;
	m_bSynMain = false;
	m_MorphPattern.SetNull();
	m_bMeta = true;
	m_Source = "<empty>";
	m_Register = AnyRegister;
	m_pListFile = NULL;
	//m_pListFile = m_TokenListFiles.end();
	m_Attributes.clear();
	m_Token = "";
	m_ItemStrId = "";
	m_TokenType = OTHER_TOKEN_TYPE;
    m_GraDescrs = 0;
};

CGrammarItem::CGrammarItem()
{
	InitGrammarItem();
};




bool CGrammarItem::operator <(const CGrammarItem& _X1) const
{

	if (m_ItemStrId != _X1.m_ItemStrId)
		return m_ItemStrId < _X1.m_ItemStrId;

	if (m_bMeta != _X1.m_bMeta)
		return m_bMeta < _X1.m_bMeta;

	bool b = (m_MorphPattern < _X1.m_MorphPattern);
	if (b)
		return b;

	if (m_bCanHaveManyHomonyms != _X1.m_bCanHaveManyHomonyms)
		return m_bCanHaveManyHomonyms < _X1.m_bCanHaveManyHomonyms;

	if (m_Register != _X1.m_Register)
		return m_Register < _X1.m_Register;

    if (m_GraDescrs != _X1.m_GraDescrs)
		return m_GraDescrs < _X1.m_GraDescrs;

	return m_Attributes < _X1.m_Attributes;
};

/*
 this operator checks  the equality of two CGrammarItems disregarding 
 working parameters like (m_Register or m_Grammems).
 It is neccessary for example to check  grammar coherence, i.e. to assure ourselves that 
 all nonterminals are somewhere declared
*/
bool CGrammarItem::RuleItemPartialEqual (const CGrammarItem& _X1)  const
{
	//const char* s1 = _X1.m_ItemStrId.c_str();
	//const char* s2 = _X2.m_ItemStrId.c_str();
	bool b = m_ItemStrId == _X1.m_ItemStrId
			&&	m_bMeta == _X1.m_bMeta
			&&	m_Attributes == _X1.m_Attributes
			;
	return b;
};

bool CGrammarItem::operator ==(const CGrammarItem& _X1) const
{
	bool b =    RuleItemPartialEqual(_X1)
			&&	m_MorphPattern == _X1.m_MorphPattern
			&&	m_bCanHaveManyHomonyms == _X1.m_bCanHaveManyHomonyms
			&&	m_Register == _X1.m_Register
            &&  m_GraDescrs == _X1.m_GraDescrs;
	return b;
};

void CGrammarItem::CopyNonEmptyWorkAttributesFrom(const CGrammarItem& Item)
{
	if (!Item.m_MorphPattern.m_GrmAttribute.empty())
	{
		m_MorphPattern.CopyFrom(Item.m_MorphPattern);
	};

	if (!Item.m_bCanHaveManyHomonyms)
		m_bCanHaveManyHomonyms = Item.m_bCanHaveManyHomonyms;

	if (Item.m_Register != AnyRegister)
		m_Register = Item.m_Register;

    if (Item.m_GraDescrs != 0)
        m_GraDescrs = Item.m_GraDescrs;
};






string CGrammarItem::GetDumpString() const 
{

	if (!m_bMeta && !m_Token.empty() && m_Attributes.empty() &&  m_MorphPattern.m_GrmAttribute.empty() ) 
		return Format("'%s'",m_ItemStrId.c_str());

	string Attributes;
	for (map<string, string>::const_iterator it = m_Attributes.begin(); it != m_Attributes.end(); it++)
		Attributes += Format ("%s=%s ",it->first.c_str(),it->second.c_str());

	if (!m_MorphPattern.m_GrmAttribute.empty())
		Attributes += Format ("grm=\"%s\" ", m_MorphPattern.m_GrmAttribute.c_str());

	if (!m_bCanHaveManyHomonyms)
		Attributes += "hom=\"no\" ";

	if (m_Register != AnyRegister)
		if (m_Register == LowLow)
			Attributes += "register=\"aa\" ";
		else
			if (m_Register == UpLow)
				Attributes += "register=\"Aa\" ";
			else
				Attributes += "register=\"AA\" ";

	if (m_TokenType != OTHER_TOKEN_TYPE)
		Attributes += Format("type=\"%s\" ", TokenTypeToString(m_TokenType).c_str());

	if (m_bSynMain)
		Attributes += "root ";


	Trim(Attributes);

	string Meta;
	if (m_bMeta)
		Meta = m_ItemStrId;
	else
		Meta = "TOKEN";


	return Format("[%s %s]",Meta.c_str(),Attributes.c_str());
};


bool	CGrammarItem::AddAttribute(string Name, string Value, MorphLanguageEnum Language, string& ErrorStr)
{
    if (Value.length() > 0) 
	    if (Value[0] == '"')
	    {
		    if ( (Value.length()<2) || (Value[Value.length() - 1] != '"'))
		    {
			    ErrorStr = Format("no matching quotation mark for attribute value \"%s\"",Value.c_str());
			    return false;
		    };
		    Value = Value.substr(1, Value.length()-2);
	    };

	if (Name == "root")
	{
		m_bSynMain = true;
		return true;
	};

	if (Name == "type")
	{
		m_TokenType = StringToTokenType(Value);
		if (m_TokenType == OTHER_TOKEN_TYPE)
		{
			ErrorStr = Format("unknown token type:%s ",Value.c_str());
			return false;
		}
	};

	if (Name == "hom")
	{
		if (Value == "yes")
			m_bCanHaveManyHomonyms = true;
		else
			if (Value == "no")
				m_bCanHaveManyHomonyms = false;
			else
			{
				ErrorStr = Format("Bad value for attribute \"hom\" (\"%s\"). It can be \"yes\" or \"no\"",Value.c_str());
				return false;
			};

		if (m_TokenType == OTHER_TOKEN_TYPE)
				m_TokenType = (Language == morphRussian) ? RLE : LLE;
		return true;
	};


	if	(Name == "grm") 
	{
		m_MorphPattern.m_GrmAttribute = Value;
		if (m_TokenType == OTHER_TOKEN_TYPE)
				m_TokenType = (Language == morphRussian) ? RLE : LLE;
		return true;
	};

	if	(Name == "form") 
	{
		m_Token = Value;
		RmlMakeUpper(m_Token, Language);
		m_ItemStrId = Value;

		if ( (m_TokenType == OTHER_TOKEN_TYPE) && !m_Token.empty())
		{
			if (ispunct((BYTE)m_Token[0]))
				m_TokenType = PUNCTUAT;
			else
			if (isdigit((BYTE)m_Token[0]))
				m_TokenType = NUM;
			else
			if (Language == morphRussian)
			{
				if (CheckLanguage(m_Token, Language))
					m_TokenType = RLE;
			}
			else
			{
				if (CheckLanguage(m_Token, Language))
					m_TokenType = LLE;
			}
		};

		return true;
	};

	if (Name == "register")
	{
		if (Value == "AA")
			m_Register = UpUp;
		else
			if (Value == "aa")
				m_Register = LowLow;
			else
			if (Value == "Aa")
				m_Register = UpLow;
			else
			{
				ErrorStr = Format("Bad value for attribute \"register\" (\"%s\"). It can be \"AA\", \"aa\" or \"Aa\"",Value.c_str());
				return false;
			};
		if (m_TokenType == OTHER_TOKEN_TYPE)
				m_TokenType = (Language == morphRussian) ? RLE : LLE;
		return true;
	};

	if (Name == "filename")
	{
		Value = GetPathByFile(CurrentSourceFileName) + Value;
		if (m_TokenType == OTHER_TOKEN_TYPE)
				m_TokenType = (Language == morphRussian) ? RLE : LLE;
	}

    if (Name == "graf")
    {
        string unparsed;
        m_GraDescrs = parse_gra_descriptors(Value.c_str(), unparsed);
        return true;
    }

	m_Attributes[Name] = Value;

    return true;
};




string	CGrammarItem::toString() const
{
	string Result = "[\n";
	Result += Format("%i %s %s %i\n", 
			(int) m_bMeta, 
			m_ItemStrId.c_str(), 
			m_Token.empty() ? "null" : m_Token.c_str(), 
			(int)m_TokenType);

	Result += Format("%s\x1\n", m_Source.c_str());

	Result += m_MorphPattern.ToString();
	Result += Format("%i %i %i %i", m_bGrammarRoot?1:0, m_bSynMain?1:0, m_bCanHaveManyHomonyms?1:0, (int)m_Register);
#ifdef WIN32
	Result += Format(" %I64i\n", m_GraDescrs);
#else
    Result += Format(" %lli\n", m_GraDescrs);
#endif
	for (map<string, string>::const_iterator i =m_Attributes.begin();  i != m_Attributes.end(); i++)
		Result += Format(";%s %s", i->first.c_str(),i->second.c_str());
	Result += ";\n";
	Result += "]\n";

	return Result;
}


bool	CGrammarItem::fromString(string& Result) 
{
	StringTokenizer lines (Result.c_str(), "\r\n");
	int LineNo = 0;
	while (lines())
	{
		LineNo++;
		string line = lines.val();
		if (LineNo == 1)
			continue;

		if (LineNo == 2)
		{
			char buff1[1024];
			char buff2[1024];
			int iMeta;
			int iTokenType;

			if (sscanf(line.c_str(),  "%i %s %s %i\n", &iMeta, buff1, buff2, &iTokenType) != 4) 
				return false;
			m_bMeta = (bool)iMeta;
			m_TokenType = (MainTokenTypeEnum)iTokenType;
			m_ItemStrId = buff1;
			if (m_ItemStrId.empty()) return false;

			m_Token = buff2;
			if (m_Token == "null")
				m_Token = "";

		};
		if (LineNo == 3)
		{
			char buff1[1024];
			int Count = sscanf(line.c_str(),  "%[^\x1]", buff1);
			if (Count != 1) return false;
			m_Source = buff1;
		};

		if (LineNo == 4)
		{
			if (!m_MorphPattern.FromString(line))
				return false;
		};
		if (LineNo == 5)
		{
    		int i1, i2,i3, i4;
#ifdef WIN32
			if (sscanf(line.c_str(),  "%i %i %i %i %I64i", &i1, &i2, &i3, &i4, &m_GraDescrs) != 5) return false;
#else
            if (sscanf(line.c_str(),  "%i %i %i %i %lli", &i1, &i2, &i3, &i4, &m_GraDescrs) != 5) return false;
#endif
			m_bGrammarRoot = i1==1;
			m_bSynMain = i2==1;
			m_bCanHaveManyHomonyms = i3==1;
			m_Register = (RegisterEnum)i4;
		};
		if (LineNo == 6)
		{
			m_Attributes.clear();
			StringTokenizer pairs(line.c_str(), ";");
			while (pairs())
			{
				char buff1[1024];
				char buff2[1024];
				string _pair = pairs.val();
				sscanf (_pair.c_str(), "%s %s", buff1, buff2);
				m_Attributes[buff1] = buff2;
			};
            map<string, string>::const_iterator it = m_Attributes.find("graf");
            if (it != m_Attributes.end())
            {
                string unparsed;
                m_GraDescrs = parse_gra_descriptors(it->second.c_str(), unparsed);
            }

		};

	};

	if (LineNo != 7) return false;

	return true;
}

bool	CGrammarItem::HasAnyOfWorkingAttributes() const
{
		return  (		!m_bCanHaveManyHomonyms
					||	!m_MorphPattern.m_GrmAttribute.empty()
					||	(m_Register != AnyRegister)
			);
};

string CGrammarItem::GetFullFileName(const string& GrammarFileName) const
{
	map<string,string>::const_iterator it = m_Attributes.find("filename");
	if (it == m_Attributes.end())  return "";
	return GetPathByFile(GrammarFileName)+it->second;
};
