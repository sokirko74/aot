#include "SimpleGrammarLib.h"
#include "GrammarItem.h"
#include "morph_dict/agramtab/agramtab.h"
#include "graphan/GraphanLib/graline.h"

void CGrammarItem::InitGrammarItem()
{
	m_bGrammarRoot = true;
	m_bCanHaveManyHomonyms = true;
	m_bSynMain = false;
	m_MorphPattern.SetNotWord();
	m_bMeta = true;
	m_Source = "<empty>";
	m_Register = AnyRegister;
	m_pListFile = nullptr;
	m_Attributes.clear();
	m_Token = "";
	m_ItemStrId = "";
	m_TokenType = OTHER_TOKEN_TYPE;
	m_GraphemDescrs = 0;
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

	if (m_GraphemDescrs != _X1.m_GraphemDescrs)
		return m_GraphemDescrs < _X1.m_GraphemDescrs;

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
		&& m_GraphemDescrs == _X1.m_GraphemDescrs
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
	
	if (Item.m_GraphemDescrs != 0)
		m_GraphemDescrs = Item.m_GraphemDescrs;

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

	if (m_GraphemDescrs) {
		auto s = CGraLine::GetStringByDescriptors(m_GraphemDescrs);
		Attributes += Format("graphem=\"%s\" ", s.c_str());
	}

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
	else if (Name == "graphem") {
		m_GraphemDescrs = parse_graphem_descriptors(Value);
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


void CGrammarItem::ToJsonObject(CJsonObject& o) const
{
	o.add_bool("meta", m_bMeta);
	o.add_string("item_str_id", m_ItemStrId);
	o.add_string("token", m_Token);
	o.add_int("token_type", m_TokenType);
	o.add_string("source", m_Source);
	CJsonObject morph(o.get_doc());
	m_MorphPattern.ToJsonObject(morph);
	o.move_to_member("morph", morph.get_value());
	o.add_bool("root", m_bGrammarRoot);
	o.add_bool("syn_main", m_bSynMain);
	o.add_bool("hom", m_bCanHaveManyHomonyms);
	o.add_int("register", m_Register);
	o.add_int64("graphem", m_GraphemDescrs);
	CJsonObject attrs(o.get_doc());
	for (auto& [key, val] : m_Attributes)
		attrs.add_string(key.c_str(), val);
	o.move_to_member("attrs", attrs.get_value());
}


void CGrammarItem::FromJsonObject(const rapidjson::Value& inj)
{
	m_bMeta = inj["meta"].GetBool();
	m_ItemStrId = inj["item_str_id"].GetString();
	m_Token = inj["token"].GetString();
	m_TokenType = (Descriptors)inj["token_type"].GetInt();
	m_Source = inj["source"].GetString();
	m_MorphPattern.FromJsonObject(inj["morph"].GetObject());
	m_bGrammarRoot = inj["root"].GetBool();
	m_bSynMain = inj["syn_main"].GetBool();
	m_bCanHaveManyHomonyms = inj["hom"].GetBool();
	m_Register = (RegisterEnum)inj["register"].GetInt();
	m_GraphemDescrs = inj["graphem"].GetInt64();
	auto& a = inj["attrs"];
	m_Attributes.clear();
	for (auto m = a.MemberBegin(); m != a.MemberEnd(); ++m) {
		m_Attributes[m->name.GetString()] = m->value.GetString();
	}
}

bool	CGrammarItem::HasAnyOfWorkingAttributes() const
{
	return  (!m_bCanHaveManyHomonyms
		|| !m_MorphPattern.HasNoInfo()
		|| (m_Register != AnyRegister)
		|| (m_GraphemDescrs != 0)
		);
};

