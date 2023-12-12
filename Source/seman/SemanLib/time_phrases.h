#pragma once

// Этот класс используется пока только для нахождения групп времени
struct CNodeHypot {
    // номер узла
    long m_NodeNo;
    // этаж в поле LEXi
    long m_LexFetNo;
    // этаж в поле GFi
    long m_GramCortegeNo;
    // номер предлога в словаре оборотов
    long m_PrepNo;
    // номер предлога в поле PREP
    long m_PrepNoInArticle;
    // верно, если  GF = ЦК_порядк для этого узла
    bool m_bShouldBeNumeral_P;

    CNodeHypot(long NodeNo, long LexFetNo, long GramCortegeNo, long PrepNo, long PrepNoInArticle,
        bool bShouldBeNumeral_P);
    CNodeHypot(long NodeNo, long GramCortegeNo);
    CNodeHypot();
};

typedef std::vector<CNodeHypot> CNodeHypotVector;


struct CTimeNodeHypot {
	long m_UnitNo;
	long m_CountOfHoles;
	long m_LengthInText;
	long m_LengthInTitle;
	long m_CountOfLexFet;
	std::vector <CNodeHypot> m_Periods;

    CTimeNodeHypot(long UnitNo, size_t CountOfHoles, const std::vector <CNodeHypot>& Periods, long LengthInText, long LengthInTitle);
	long GetWeightOfTimeNode() const;
	bool operator < (const CTimeNodeHypot& X) const;
    bool operator == (const CTimeNodeHypot& X) const;
};
