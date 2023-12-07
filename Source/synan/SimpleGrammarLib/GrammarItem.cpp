#include "SimpleGrammarLib.h"
#include "GrammarItem.h"
#include "morph_dict/agramtab/agramtab.h"

void CGrammarItem::InitGrammarItem()
{
	m_bGrammarRoot = true;
	m_bCanHaveManyHomonyms = true;
	m_bSynMain = false;
	m_MorphPattern.SetNotWord();
	m_bMeta = true;
	m_Source = "<empty>";
	m_Register = AnyRegister;
	m_pListFile = NULL;
	//m_pListFile = m_TokenListFiles.end();
	m_Attributes.clear();
	m_Token = "";
	m_ItemStrId = "";
	m_TokenType = OTHER_TOKEN_TYPE;
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

	return m_Attributes < _X1.m_Attributes;
};

/*
 this operator checks  the equality of two CGrammarItems disregarding
 working parameters like (m_Register or m_Grammems).
 It is neccessary for example to check  grammar coherence, i.e. to assure ourselves that
 all nonterminals are somewhere declared
*/
bool CGrammarItem::RuleItemPartialEqual(const CGrammarItem& _X1)  const
{
	//const char* s1 = _X1.m_ItemStrId.c_str();
	//const char* s2 = _X2.m_ItemStrId.c_str();
	bool b = m_ItemStrId == _X1.m_ItemStrId
		&& m_bMeta == _X1.m_bMeta
		&& m_Attributes == _X1.m_Attributes
		;
	return b;
};

bool CGrammarItem::operator ==(const CGrammarItem& _X1) const
{
	bool b = RuleItemPartialEqual(_X1)
		&& m_MorphPattern == _X1.m_MorphPattern
		&& m_bCanHaveManyHomonyms == _X1.m_bCanHaveManyHomonyms
		&& m_Register == _X1.m_Register;
	return b;
};

void CGrammarItem::CopyNonEmptyWorkAttributesFrom(const CGrammarItem& Item)
{
	if (!Item.m_MorphPattern.HasNoInfo())
	{
		m_MorphPattern = Item.m_MorphPattern;
	};

	if (!Item.m_bCanHaveManyHomonyms)
		m_bCanHaveManyHomonyms = Item.m_bCanHaveManyHomonyms;

	if (Item.m_Register != AnyRegister)
		m_Register = Item.m_Register;
};






std::string CGrammarItem::GetDumpString() const
{

	if (!m_bMeta && !m_Token.empty() && m_Attributes.empty() && m_MorphPattern.HasNoInfo())
		return Format("'%s'", m_ItemStrId.c_str());

	std::string Attributes;
	for (std::map<std::string, std::string>::const_iterator it = m_Attributes.begin(); it != m_Attributes.end(); it++)
		Attributes += Format("%s=%s ", it->first.c_str(), it->second.c_str());

	if (!m_MorphPattern.HasNoInfo())
		Attributes += Format("grm=\"%s\" ", m_MorphPattern.ToGrammarFormat().c_str());

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
		Attributes += Format("type=\"%s\" ", GetDescriptorStr(m_TokenType).c_str());

	if (m_bSynMain)
		Attributes += "root ";


	Trim(Attributes);

	std::string Meta;
	if (m_bMeta)
		Meta = m_ItemStrId;
	else
		Meta = "TOKEN";


	return Format("[%s %s]", Meta.c_str(), Attributes.c_str());
};

static Descriptors StringToTokenType(const  std::string& t)
{
	if (t == "RLE")	return ORLE;
	if (t == "LLE")	return OLLE;
	if (t == "DC")	return ODigits;
	if (t == "ROMAN")	return ORoman;
	if (t == "DSC")	return ONumChar;
	if (t == "PUN")	return OPun;
	return OTHER_TOKEN_TYPE;
};


void CGrammarItem::AddAttribute(std::string Name, std::string Value, MorphLanguageEnum Language, std::string& ErrorStr, const std::string& SourceFileName)
{
	if (!Value.empty())
		if (Value[0] == '"')
		{
			if ((Value.length() < 2) || (Value.back() != '"'))
			{
				throw CExpc("no matching quotation mark for attribute value \"%s\"", Value.c_str());
			};
			Value = Value.substr(1, Value.length() - 2);
		};

	if (Name == "root")
	{
		m_bSynMain = true;
		return;
	}
	else if (Name == "type")
	{
		m_TokenType = StringToTokenType(Value);
		if (m_TokenType == OTHER_TOKEN_TYPE)
		{
			throw CExpc("unknown token type:%s ", Value.c_str());
		}
	}
	else if (Name == "hom")
	{
		if (Value == "yes")
			m_bCanHaveManyHomonyms = true;
		else
			if (Value == "no")
				m_bCanHaveManyHomonyms = false;
			else
			{
				throw CExpc("Bad value for attribute \"hom\" (\"%s\"). It can be \"yes\" or \"no\"", Value.c_str());
			};

		if (m_TokenType == OTHER_TOKEN_TYPE)
			m_TokenType = (Language == morphRussian) ? ORLE : OLLE;
		return;
	}
	else if (Name == "grm")
	{
		m_MorphPattern.InitFromGrammarFormat(Language, Value);
		if (m_TokenType == OTHER_TOKEN_TYPE)
			m_TokenType = (Language == morphRussian) ? ORLE : OLLE;
		return;
	}
	else  if (Name == "form")
	{
		m_Token = Value;
		MakeUpperUtf8(m_Token);
		m_ItemStrId = Value;

		if ((m_TokenType == OTHER_TOKEN_TYPE) && !m_Token.empty())
		{
			if (m_Token[0] == '*' && m_Token.length() > 1) {
				if (Language == morphRussian) {
					m_TokenType = ORLE;
				}
				else {
					m_TokenType = OLLE;
				}
			}
			else if (std::iswpunct(m_Token[0]))
				m_TokenType = OPun;
			else
				if (isdigit((BYTE)m_Token[0]))
					m_TokenType = ODigits;
				else
					if (Language == morphRussian)
					{
						if (CheckRussianUtf8(m_Token))
							m_TokenType = ORLE;
					}
					else if (Language == morphGerman)
					{
						if (CheckGermanUtf8(m_Token))
							m_TokenType = OLLE;

						else
						{
							LOGI << "use english abc for unknown language";
							if (CheckEnglishUtf8(m_Token))
								m_TokenType = OLLE;
						}
					};
			return;
		}
	}
	else if (Name == "register")
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
					throw CExpc("Bad value for attribute \"register\" (\"%s\"). It can be \"AA\", \"aa\" or \"Aa\"", Value.c_str());
				};
		if (m_TokenType == OTHER_TOKEN_TYPE)
			m_TokenType = (Language == morphRussian) ? ORLE : OLLE;
		return;
	}
	else if (Name == "filename")
	{
		// SourceFileName  is the file where this grammar item was read from 
		Value = MakePath(GetParentPath(SourceFileName), Value);
		if (!FileExists(Value.c_str())) {
			throw CExpc("file %s does not exist", Value.c_str());
		}
		if (m_TokenType == OTHER_TOKEN_TYPE)
			m_TokenType = (Language == morphRussian) ? ORLE : OLLE;
	}
	m_Attributes[Name] = Value;
};


std::string	CGrammarItem::toString() const
{
	std::string Result = "[\n";
	Result += Format("%i %s %s %i\n",
		(int)m_bMeta,
		m_ItemStrId.c_str(),
		m_Token.empty() ? "null" : m_Token.c_str(),
		(int)m_TokenType);

	Result += Format("%s\x1\n", m_Source.c_str());

	Result += m_MorphPattern.ToString() + "\n";
	Result += Format("%i %i %i %i\n", m_bGrammarRoot ? 1 : 0, m_bSynMain ? 1 : 0, m_bCanHaveManyHomonyms ? 1 : 0, (int)m_Register);

	for (std::map<std::string, std::string>::const_iterator i = m_Attributes.begin(); i != m_Attributes.end(); i++)
		Result += Format(";%s %s", i->first.c_str(), i->second.c_str());
	Result += ";\n";
	Result += "]\n";

	return Result;
}


bool	CGrammarItem::fromString(std::string & Result)
{
	StringTokenizer lines(Result.c_str(), "\r\n");
	int LineNo = 0;
	while (lines())
	{
		LineNo++;
		std::string line = lines.val();
		if (LineNo == 1)
			continue;

		if (LineNo == 2)
		{
			char buff1[1024];
			char buff2[1024];
			int iMeta;
			int iTokenType;
			if (sscanf(line.c_str(), "%i %s %s %i\n", &iMeta, buff1, buff2, &iTokenType) != 4)
				return false;
			m_bMeta = (bool)iMeta;
			m_TokenType = (Descriptors)iTokenType;
			m_ItemStrId = buff1;
			if (m_ItemStrId.empty()) return false;

			m_Token = buff2;
			if (m_Token == "null")
				m_Token = "";

		};
		if (LineNo == 3)
		{
			char buff1[1024];
			int Count = sscanf(line.c_str(), "%[^\x1]", buff1);
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
			int i1, i2, i3, i4;
			if (sscanf(line.c_str(), "%i %i %i %i", &i1, &i2, &i3, &i4) != 4) return false;
			m_bGrammarRoot = i1 == 1;
			m_bSynMain = i2 == 1;
			m_bCanHaveManyHomonyms = i3 == 1;
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
				std::string _pair = pairs.val();
				sscanf(_pair.c_str(), "%s %s", buff1, buff2);
				m_Attributes[buff1] = buff2;
			};

		};

	};

	if (LineNo != 7) return false;

	return true;
}

bool	CGrammarItem::HasAnyOfWorkingAttributes() const
{
	return  (!m_bCanHaveManyHomonyms
		|| !m_MorphPattern.HasNoInfo()
		|| (m_Register != AnyRegister)
		);
};

