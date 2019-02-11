#include  "MorphXmlToken.h"
#include  "util_classes.h"
#include  <set>

void	CXmlMorphAnnot::GetAsSetOfProperties(vector<string>& Result)  const
{
	Result.clear();

	StringTokenizer tok2(m_GrammemsStr.c_str(), " \t,|; ");
	while (tok2())
		Result.push_back(tok2.val());

	sort(Result.begin(), Result.end());

};


CXmlToken::CXmlToken() 
{
	m_bLastInSentence = false;
};

bool CXmlToken::IsImportantPunct() const
{
    return                  (m_WordStr == ",") 
						||	(m_WordStr == ".")
						||	(m_WordStr == "?")
						||	(m_WordStr == "!")
						||	(m_WordStr == ":")
						||	(m_WordStr == ";")
						||	(m_WordStr == "-");
				
}


class CRusCorpFeatures {
public:
    std::set<std::string> m_RusCorpFeatures;
    CRusCorpFeatures()
    {
        m_RusCorpFeatures.insert("adv");
        m_RusCorpFeatures.insert("a");
        m_RusCorpFeatures.insert("s");
        m_RusCorpFeatures.insert("v");
        m_RusCorpFeatures.insert("spro");
        m_RusCorpFeatures.insert("apro");
        m_RusCorpFeatures.insert("advpro");
        m_RusCorpFeatures.insert("num");
        m_RusCorpFeatures.insert("anum");
        m_RusCorpFeatures.insert("praedic");
        m_RusCorpFeatures.insert("pr");
        m_RusCorpFeatures.insert("conj");
        m_RusCorpFeatures.insert("intj");
        m_RusCorpFeatures.insert("parenth");
        m_RusCorpFeatures.insert("part");
        m_RusCorpFeatures.insert("nonlex");
        m_RusCorpFeatures.insert("init");
        m_RusCorpFeatures.insert("praedicpro");
        m_RusCorpFeatures.insert("sg");
        m_RusCorpFeatures.insert("pl");
        m_RusCorpFeatures.insert("nom");
        m_RusCorpFeatures.insert("gen");
        m_RusCorpFeatures.insert("dat");
        m_RusCorpFeatures.insert("acc");
        m_RusCorpFeatures.insert("ins");
        m_RusCorpFeatures.insert("loc");
        m_RusCorpFeatures.insert("voc");
        m_RusCorpFeatures.insert("m");
        m_RusCorpFeatures.insert("f");
        m_RusCorpFeatures.insert("n");
        m_RusCorpFeatures.insert("loc2");
        m_RusCorpFeatures.insert("gen2");
        m_RusCorpFeatures.insert("acc2");
        m_RusCorpFeatures.insert("famn");
        m_RusCorpFeatures.insert("persn");
        m_RusCorpFeatures.insert("patrn");
        m_RusCorpFeatures.insert("imper");
        m_RusCorpFeatures.insert("inf");
        m_RusCorpFeatures.insert("partcp");
        m_RusCorpFeatures.insert("ger");
        m_RusCorpFeatures.insert("imper2");
        m_RusCorpFeatures.insert("1p");
        m_RusCorpFeatures.insert("2p");
        m_RusCorpFeatures.insert("3p");
        m_RusCorpFeatures.insert("praes");
        m_RusCorpFeatures.insert("fut");
        m_RusCorpFeatures.insert("praet");
        m_RusCorpFeatures.insert("anim");
        m_RusCorpFeatures.insert("inan");
        m_RusCorpFeatures.insert("mf");
        m_RusCorpFeatures.insert("abbr");
        m_RusCorpFeatures.insert("pf");
        m_RusCorpFeatures.insert("ipf");
        m_RusCorpFeatures.insert("comp");
        m_RusCorpFeatures.insert("brev");
        m_RusCorpFeatures.insert("pass");
        m_RusCorpFeatures.insert("0");
        m_RusCorpFeatures.insert("imper2");
        //m_RusCorpFeatures.insert("bastard");
        m_RusCorpFeatures.insert("supr");
    }
};

static CRusCorpFeatures  RusCorpFeatures;

bool IsImportantRusCorpFeature(const string& s)
{
    return RusCorpFeatures.m_RusCorpFeatures.find(s) !=  RusCorpFeatures.m_RusCorpFeatures.end();
}

string CXmlMorphAnnot::BuildRusCorpAnnot() const
{
    StringTokenizer tok (m_GrammemsStr.c_str(), ", ");
    set<string> gs;
    while ( tok() )
    {
        string s = tok.val();
        EngRusMakeLower(s);
        if (IsImportantRusCorpFeature (s))
            gs.insert(s);
    }

    string res;
    for (set<string>::const_iterator it = gs.begin(); it != gs.end(); it++)
        res += *it + "_";
    if (!res.empty())
        res.erase(res.length() -1);
    return res;
}

string POS_Dialing2MyStem(string pos)
{
	if (pos == "�") return "a"; else
	if (pos == "�") return "s"; else
	if (pos == "�") return "v"; else
	if (pos == "��") return "s-pro"; else
	if (pos == "��-�") return "a-pro"; else
	if (pos == "��-�����") return "praedicpro"; else
	if (pos == "����") return "num"; else
	if (pos == "����-�") return "a-num"; else
	if (pos == "�") return "adv"; else
	if (pos == "�����") return "praedic"; else
	if (pos == "�����") return "pr"; else
	if (pos == "����") return "conj"; else
	if (pos == "����") return "intj"; else
	if (pos == "�����") return "parenth"; else
	if (pos == "����") return "part"; else
	if (pos == "������_����") return "conj"; else
	{
		return "";
	}
};


string Grammem_Dialing2MyStem(string pos)
{
	if (pos == "��") return "sg"; else
	if (pos == "��") return "pl"; else
	if (pos == "��") return "nom";  else
	if (pos == "��") return "gen";  else
	if (pos == "��") return "dat";  else
	if (pos == "��") return "acc";  else
	if (pos == "��") return "ins";  else
	if (pos == "��") return "loc";  else
	if (pos == "��") return "voc";  else
	if (pos == "��") return "m";  else
	if (pos == "��") return "f";  else
	if (pos == "��") return "n";  else
	if (pos == "���") return "famn";  else
	if (pos == "���") return "persn";  else
	if (pos == "���") return "patrn";  else
	if (pos == "���") return "imper";  else
	if (pos == "���") return "inf";  else
	if (pos == "���") return "partcp";  else
	if (pos == "���") return "ger";  else
	if (pos == "1�") return "1p";  else
	if (pos == "2�") return "2p";  else
	if (pos == "3�") return "3p";  else
	if (pos == "�����") return "comp";  else
	if (pos == "��") return "brev";  else
	if (pos == "���") return "praes";  else
	if (pos == "���") return "fut";  else
	if (pos == "���") return "praet";  else
	if (pos == "���") return "act";  else
	if (pos == "���") return "pass";  else
	if (pos == "��") return "anim";  else
	if (pos == "��") return "inan";  else
	if (pos == "��") return "pf";  else
	if (pos == "��") return "ipf";  else
	if (pos == "��") return "tran";  else
	if (pos == "��") return "intr";  else
	if (pos == "��-��") return "m-f";  else
	if (pos == "����") return "anom";  else
	if (pos == "����") return "anom";  else
	if (pos == "����") return "supr";  else
	if (pos == "0") return "0";  else
    if (pos == "��-��") return "mf";  else
	if (pos == "����") return "abbr";  else
	{
		return "";
	}
	
	return "";

};

string POS_MyStem2Dialing(string pos)
{
	if (pos == "a") return "�"; else
	if (pos == "s") return "�"; else
	if (pos == "v") return "�"; else
	if (pos == "spro") return "��"; else
	if (pos == "s-pro") return "��"; else
	if (pos == "apro") return "��-�"; else
	if (pos == "a-pro") return "��-�"; else
	if (pos == "advpro") return "�"; else
	if (pos == "praedicpro") return "��-�����"; else
	if (pos == "num") return "����"; else
	if (pos == "anum") return "����-�"; else
	if (pos == "a-num") return "����-�"; else
	if (pos == "adv") return "�"; else
	if (pos == "praedic") return "�����"; else
	if (pos == "pr") return "�����"; else
	if (pos == "conj") return "����"; else
	if (pos == "intj") return "����"; else
	if (pos == "parenth") return "�����"; else
	if (pos == "part") return "����"; else
	if (pos == "�") return "H"; else
	if (pos == "�") return "�"; else
	if (pos == "����") return "����"; else
	if (pos == "����") return "����"; else
	if (pos == "�-�����") return "�C-�����"; else
	if (pos == "������_����") return "����"; else
	return "";
};


bool IsMyStemSpecificGrammem(string g)
{
return 		(g == "indic") 
		||	(g == "ash") 
        ||	(g == "Ash") 
		||	(g == "norm") 
		||	(g == "anom") 
		||	(g == "distort") 
		||	(g == "bastard") 
		||	(g == "med") 
		||	(g == "plen")
		||	(g == "ciph")
		||	(g == "adnum")
		||	(g == "obsc")
		;
}

string Grammem_MyStem2Dialing(string pos)
{
	if (pos == "sg") return "��"; else
	if (pos == "pl") return "��"; else
	if (pos == "nom") return "��";  else
	if (pos == "gen") return "��";  else
	if (pos == "gen2") return "��,2";  else
	if (pos == "dat") return "��";  else
	if (pos == "acc") return "��";  else
	if (pos == "acc2") return "��";  else // "����� � �������"
	if (pos == "ins") return "��";  else
	if (pos == "loc") return "��";  else
	if (pos == "loc2") return "��,2";  else
	if (pos == "m") return "��";  else
	if (pos == "f") return "��";  else
    if (pos == "mf") return "��-��";  else
	if (pos == "n") return "��";  else

	if (pos == "famn") return "���";  else
	if (pos == "persn") return "���";  else
	if (pos == "patrn") return "���";  else
	if (pos == "imper") return "���";  else
	if (pos == "imper2") return "���";  else
	if (pos == "inf") return "���";  else
	if (pos == "partcp") return "���";  else
	if (pos == "ger") return "���";  else
	if (pos == "1p") return "1�";  else
	if (pos == "1-�") return "1�";  else
	if (pos == "2p") return "2�";  else
	if (pos == "3p") return "3�";  else
	if (pos == "comp") return "�����";  else
	if (pos == "comp2") return "�����,2";  else
	if (pos == "supr") return "����";  else
	if (pos == "����") return "����";  else
	if (pos == "brev") return "��";  else
	if (pos == "sh") return "��";  else
	if (pos == "praes") return "���";  else
	if (pos == "abbr") return "����";  else
	if (pos == "����*") return "���";  else
	if (pos == "fut") return "���";  else
	if (pos == "praet") return "���";  else
	if (pos == "act") return "���";  else
	if (pos == "pass") return "���";  else
	if (pos == "anim") return "��";  else
	if (pos == "inan") return "��";  else
	if (pos == "pf") return "��";  else
	if (pos == "ipf") return "��";  else
	if (pos == "tran") return "��";  else
	if (pos == "intr") return "��";  else
	if (pos == "m-f") return "��-��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "���") return "���";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "�") return "��";  else

	if (pos == "���") return "���";  else
	
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "���") return "���";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "��") return "��";  else
	if (pos == "voc") return "��";  else
	if (pos == "0") return "0";  else
	if (IsMyStemSpecificGrammem(pos)) return pos;  else
	
		
	
	return "";

};


