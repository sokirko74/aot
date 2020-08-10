#pragma once

struct CMorphForm 
{
	std::string m_Gramcode;
	std::string m_FlexiaStr;
	std::string m_PrefixStr;

	CMorphForm (std::string Gramcode, std::string FlexiaStr, std::string PrefixStr)
	{
		m_Gramcode = Gramcode;
		m_FlexiaStr = FlexiaStr;
		m_PrefixStr = PrefixStr;
		assert (!m_Gramcode.empty());
	};
	bool		operator == (const CMorphForm& X) const 
	{
		return			m_Gramcode == X.m_Gramcode
					&&	m_FlexiaStr == X.m_FlexiaStr
					&&	m_PrefixStr == X.m_PrefixStr;
	};

};

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
struct  CFlexiaModel
{
	std::string				m_Comments;
	vector<CMorphForm>	m_Flexia;

	bool		operator == (const CFlexiaModel& X) const 
	{
		return	m_Flexia == X.m_Flexia;
	};
	bool		ReadFromString(std::string& s);
	std::string		ToString() const;

	std::string		get_first_flex() const;
	std::string		get_first_code() const;
	bool		has_ancode(const std::string& search_ancode) const;
	
};



//----------------------------------------------------------------------------
struct CAccentModel 
{
	vector<BYTE> m_Accents;

	bool operator == (const CAccentModel& X) const
	{
		return m_Accents == X.m_Accents;

	};
	bool		ReadFromString(const std::string& s);
	std::string		ToString() const;
};


//----------------------------------------------------------------------------

const size_t CommonAncodeSize = 2;
const WORD UnknownParadigmNo = 0xffff-1;
const WORD UnknownAccentModelNo = 0xffff-1;

struct CLemmaInfo
{
	WORD	m_FlexiaModelNo;
	WORD	m_AccentModelNo;
	char	m_CommonAncode[CommonAncodeSize];

	CLemmaInfo()
	{
		m_FlexiaModelNo = UnknownParadigmNo;
		m_AccentModelNo = UnknownAccentModelNo;
		m_CommonAncode[0] = 0; 
	};

	bool operator ==(const CLemmaInfo& obj) const
	{	
		return (	(m_FlexiaModelNo == obj.m_FlexiaModelNo)
				&&	(m_AccentModelNo == obj.m_AccentModelNo)
				&&	!strncmp(m_CommonAncode,obj.m_CommonAncode,CommonAncodeSize)
				);	
	}

	bool operator <(const CLemmaInfo& obj) const
	{	
		if (m_FlexiaModelNo != obj.m_FlexiaModelNo)
			return m_FlexiaModelNo < obj.m_FlexiaModelNo;

		int res = strncmp(m_CommonAncode,obj.m_CommonAncode,CommonAncodeSize);
		if (res != 0)
			return res < 0;

		return m_AccentModelNo < obj.m_AccentModelNo;
	}

	std::string GetCommonAncodeIfCan()  const 
	{
		if (m_CommonAncode[0] == 0) return "";
		return std::string(m_CommonAncode,2);	
	}

};
extern void ReadFlexiaModels(std::ifstream&, vector<CFlexiaModel>& FlexiaModels );
extern void WriteFlexiaModels(FILE* out_fp, const vector<CFlexiaModel>& FlexiaModels );
extern void ReadAccentModels (std::ifstream&, vector<CAccentModel>&	AccentModels );
extern void WriteAccentModels(FILE* out_fp, const vector<CAccentModel>& AccentModels );
