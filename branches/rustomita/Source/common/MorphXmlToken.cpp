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
	if (pos == "П") return "a"; else
	if (pos == "С") return "s"; else
	if (pos == "Г") return "v"; else
	if (pos == "МС") return "s-pro"; else
	if (pos == "МС-П") return "a-pro"; else
	if (pos == "МС-ПРЕДК") return "praedicpro"; else
	if (pos == "ЧИСЛ") return "num"; else
	if (pos == "ЧИСЛ-П") return "a-num"; else
	if (pos == "Н") return "adv"; else
	if (pos == "ПРЕДК") return "praedic"; else
	if (pos == "ПРЕДЛ") return "pr"; else
	if (pos == "СОЮЗ") return "conj"; else
	if (pos == "МЕЖД") return "intj"; else
	if (pos == "ВВОДН") return "parenth"; else
	if (pos == "ЧАСТ") return "part"; else
	if (pos == "РАЗРЫВ_СОЮЗ") return "conj"; else
	{
		return "";
	}
};


string Grammem_Dialing2MyStem(string pos)
{
	if (pos == "ед") return "sg"; else
	if (pos == "мн") return "pl"; else
	if (pos == "им") return "nom";  else
	if (pos == "рд") return "gen";  else
	if (pos == "дт") return "dat";  else
	if (pos == "вн") return "acc";  else
	if (pos == "тв") return "ins";  else
	if (pos == "пр") return "loc";  else
	if (pos == "зв") return "voc";  else
	if (pos == "мр") return "m";  else
	if (pos == "жр") return "f";  else
	if (pos == "ср") return "n";  else
	if (pos == "фам") return "famn";  else
	if (pos == "имя") return "persn";  else
	if (pos == "отч") return "patrn";  else
	if (pos == "пвл") return "imper";  else
	if (pos == "инф") return "inf";  else
	if (pos == "прч") return "partcp";  else
	if (pos == "дпр") return "ger";  else
	if (pos == "1л") return "1p";  else
	if (pos == "2л") return "2p";  else
	if (pos == "3л") return "3p";  else
	if (pos == "сравн") return "comp";  else
	if (pos == "кр") return "brev";  else
	if (pos == "нст") return "praes";  else
	if (pos == "буд") return "fut";  else
	if (pos == "прш") return "praet";  else
	if (pos == "дст") return "act";  else
	if (pos == "стр") return "pass";  else
	if (pos == "од") return "anim";  else
	if (pos == "но") return "inan";  else
	if (pos == "св") return "pf";  else
	if (pos == "нс") return "ipf";  else
	if (pos == "пе") return "tran";  else
	if (pos == "нп") return "intr";  else
	if (pos == "мр-жр") return "m-f";  else
	if (pos == "жарг") return "anom";  else
	if (pos == "разг") return "anom";  else
	if (pos == "прев") return "supr";  else
	if (pos == "0") return "0";  else
    if (pos == "мр-жр") return "mf";  else
	if (pos == "аббр") return "abbr";  else
	{
		return "";
	}
	
	return "";

};

string POS_MyStem2Dialing(string pos)
{
	if (pos == "a") return "П"; else
	if (pos == "s") return "С"; else
	if (pos == "v") return "Г"; else
	if (pos == "spro") return "МС"; else
	if (pos == "s-pro") return "МС"; else
	if (pos == "apro") return "МС-П"; else
	if (pos == "a-pro") return "МС-П"; else
	if (pos == "advpro") return "Н"; else
	if (pos == "praedicpro") return "МС-ПРЕДК"; else
	if (pos == "num") return "ЧИСЛ"; else
	if (pos == "anum") return "ЧИСЛ-П"; else
	if (pos == "a-num") return "ЧИСЛ-П"; else
	if (pos == "adv") return "Н"; else
	if (pos == "praedic") return "ПРЕДК"; else
	if (pos == "pr") return "ПРЕДЛ"; else
	if (pos == "conj") return "СОЮЗ"; else
	if (pos == "intj") return "МЕЖД"; else
	if (pos == "parenth") return "ВВОДН"; else
	if (pos == "part") return "ЧАСТ"; else
	if (pos == "н") return "H"; else
	if (pos == "П") return "П"; else
	if (pos == "ЧАСТ") return "ЧАСТ"; else
	if (pos == "МЕЖД") return "МЕЖД"; else
	if (pos == "М-ПРЕДК") return "МC-ПРЕДК"; else
	if (pos == "РАЗРЫВ_СОЮЗ") return "СОЮЗ"; else
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
	if (pos == "sg") return "ед"; else
	if (pos == "pl") return "мн"; else
	if (pos == "nom") return "им";  else
	if (pos == "gen") return "рд";  else
	if (pos == "gen2") return "рд,2";  else
	if (pos == "dat") return "дт";  else
	if (pos == "acc") return "вн";  else
	if (pos == "acc2") return "им";  else // "пошел в солдаты"
	if (pos == "ins") return "тв";  else
	if (pos == "loc") return "пр";  else
	if (pos == "loc2") return "пр,2";  else
	if (pos == "m") return "мр";  else
	if (pos == "f") return "жр";  else
    if (pos == "mf") return "мр-жр";  else
	if (pos == "n") return "ср";  else

	if (pos == "famn") return "фам";  else
	if (pos == "persn") return "имя";  else
	if (pos == "patrn") return "отч";  else
	if (pos == "imper") return "пвл";  else
	if (pos == "imper2") return "пвл";  else
	if (pos == "inf") return "инф";  else
	if (pos == "partcp") return "прч";  else
	if (pos == "ger") return "дпр";  else
	if (pos == "1p") return "1л";  else
	if (pos == "1-л") return "1л";  else
	if (pos == "2p") return "2л";  else
	if (pos == "3p") return "3л";  else
	if (pos == "comp") return "сравн";  else
	if (pos == "comp2") return "сравн,2";  else
	if (pos == "supr") return "прев";  else
	if (pos == "прев") return "прев";  else
	if (pos == "brev") return "кр";  else
	if (pos == "sh") return "кр";  else
	if (pos == "praes") return "нст";  else
	if (pos == "abbr") return "аббр";  else
	if (pos == "наст*") return "нст";  else
	if (pos == "fut") return "буд";  else
	if (pos == "praet") return "прш";  else
	if (pos == "act") return "дст";  else
	if (pos == "pass") return "стр";  else
	if (pos == "anim") return "од";  else
	if (pos == "inan") return "но";  else
	if (pos == "pf") return "св";  else
	if (pos == "ipf") return "нс";  else
	if (pos == "tran") return "пе";  else
	if (pos == "intr") return "нп";  else
	if (pos == "m-f") return "мр-жр";  else
	if (pos == "мн") return "мн";  else
	if (pos == "рд") return "рд";  else
	if (pos == "ед") return "ед";  else
	if (pos == "но") return "но";  else
	if (pos == "дст") return "дст";  else
	if (pos == "кр") return "кр";  else
	if (pos == "вн") return "вн";  else
	if (pos == "тв") return "тв";  else
	if (pos == "пр") return "пр";  else
	if (pos == "од") return "од";  else
	if (pos == "и") return "им";  else

	if (pos == "пвл") return "пвл";  else
	
	if (pos == "мр") return "мр";  else
	if (pos == "жр") return "жр";  else
	if (pos == "ср") return "ср";  else
	if (pos == "стр") return "стр";  else
	if (pos == "пе") return "пе";  else
	if (pos == "им") return "им";  else
	if (pos == "ср") return "ср";  else
	if (pos == "voc") return "зв";  else
	if (pos == "0") return "0";  else
	if (IsMyStemSpecificGrammem(pos)) return pos;  else
	
		
	
	return "";

};


