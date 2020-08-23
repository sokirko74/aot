#include  "MorphXmlToken.h"
#include  "util_classes.h"
#include <set>

void	CXmlMorphAnnot::GetAsSetOfProperties(std::vector<std::string>& Result)  const
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

bool IsImportantRusCorpFeature(const std::string& s)
{
    return RusCorpFeatures.m_RusCorpFeatures.find(s) !=  RusCorpFeatures.m_RusCorpFeatures.end();
}

std::string CXmlMorphAnnot::BuildRusCorpAnnot() const
{
    StringTokenizer tok (m_GrammemsStr.c_str(), ", ");
    std::set<std::string> gs;
    while ( tok() )
    {
        std::string s = tok.val();
        EngRusMakeLower(s);
        if (IsImportantRusCorpFeature (s))
            gs.insert(s);
    }

    std::string res;
    for (std::set<std::string>::const_iterator it = gs.begin(); it != gs.end(); it++)
        res += *it + "_";
    if (!res.empty())
        res.erase(res.length() -1);
    return res;
}

std::string POS_Dialing2MyStem(std::string pos)
{
	if (pos == _R("П")) return "a"; else
	if (pos == _R("С")) return "s"; else
	if (pos == _R("Г")) return "v"; else
	if (pos == _R("МС")) return "s-pro"; else
	if (pos == _R("МС-П")) return "a-pro"; else
	if (pos == _R("МС-ПРЕДК")) return "praedicpro"; else
	if (pos == _R("ЧИСЛ")) return "num"; else
	if (pos == _R("ЧИСЛ-П")) return "a-num"; else
	if (pos == _R("Н")) return "adv"; else
	if (pos == _R("ПРЕДК")) return "praedic"; else
	if (pos == _R("ПРЕДЛ")) return "pr"; else
	if (pos == _R("СОЮЗ")) return "conj"; else
	if (pos == _R("МЕЖД")) return "intj"; else
	if (pos == _R("ВВОДН")) return "parenth"; else
	if (pos == _R("ЧАСТ")) return "part"; else
	if (pos == _R("РАЗРЫВ_СОЮЗ")) return "conj"; else
	{
		return "";
	}
};


std::string Grammem_Dialing2MyStem(std::string pos)
{
	if (pos == _R("ед")) return "sg"; else
	if (pos == _R("мн")) return "pl"; else
	if (pos == _R("им")) return "nom";  else
	if (pos == _R("рд")) return "gen";  else
	if (pos == _R("дт")) return "dat";  else
	if (pos == _R("вн")) return "acc";  else
	if (pos == _R("тв")) return "ins";  else
	if (pos == _R("пр")) return "loc";  else
	if (pos == _R("зв")) return "voc";  else
	if (pos == _R("мр")) return "m";  else
	if (pos == _R("жр")) return "f";  else
	if (pos == _R("ср")) return "n";  else
	if (pos == _R("фам")) return "famn";  else
	if (pos == _R("имя")) return "persn";  else
	if (pos == _R("отч")) return "patrn";  else
	if (pos == _R("пвл")) return "imper";  else
	if (pos == _R("инф")) return "inf";  else
	if (pos == _R("прч")) return "partcp";  else
	if (pos == _R("дпр")) return "ger";  else
	if (pos == _R("1л")) return "1p";  else
	if (pos == _R("2л")) return "2p";  else
	if (pos == _R("3л")) return "3p";  else
	if (pos == _R("сравн")) return "comp";  else
	if (pos == _R("кр")) return "brev";  else
	if (pos == _R("нст")) return "praes";  else
	if (pos == _R("буд")) return "fut";  else
	if (pos == _R("прш")) return "praet";  else
	if (pos == _R("дст")) return "act";  else
	if (pos == _R("стр")) return "pass";  else
	if (pos == _R("од")) return "anim";  else
	if (pos == _R("но")) return "inan";  else
	if (pos == _R("св")) return "pf";  else
	if (pos == _R("нс")) return "ipf";  else
	if (pos == _R("пе")) return "tran";  else
	if (pos == _R("нп")) return "intr";  else
	if (pos == _R("мр-жр")) return "m-f";  else
	if (pos == _R("жарг")) return "anom";  else
	if (pos == _R("разг")) return "anom";  else
	if (pos == _R("прев")) return "supr";  else
	if (pos == _R("0")) return _R("0");  else
    if (pos == _R("мр-жр")) return "mf";  else
	if (pos == _R("аббр")) return "abbr";  else
	{
		return "";
	}
	
	return "";

};

std::string POS_MyStem2Dialing(std::string pos)
{
	if (pos == "a") return _R("П"); else
	if (pos == "s") return _R("С"); else
	if (pos == "v") return _R("Г"); else
	if (pos == "spro") return _R("МС"); else
	if (pos == "s-pro") return _R("МС"); else
	if (pos == "apro") return _R("МС-П"); else
	if (pos == "a-pro") return _R("МС-П"); else
	if (pos == "advpro") return _R("Н"); else
	if (pos == "praedicpro") return _R("МС-ПРЕДК"); else
	if (pos == "num") return _R("ЧИСЛ"); else
	if (pos == "anum") return _R("ЧИСЛ-П"); else
	if (pos == "a-num") return _R("ЧИСЛ-П"); else
	if (pos == "adv") return _R("Н"); else
	if (pos == "praedic") return _R("ПРЕДК"); else
	if (pos == "pr") return _R("ПРЕДЛ"); else
	if (pos == "conj") return _R("СОЮЗ"); else
	if (pos == "intj") return _R("МЕЖД"); else
	if (pos == "parenth") return _R("ВВОДН"); else
	if (pos == "part") return _R("ЧАСТ"); else
	if (pos == _R("н")) return _R("H"); else
	if (pos == _R("П")) return _R("П"); else
	if (pos == _R("ЧАСТ")) return _R("ЧАСТ"); else
	if (pos == _R("МЕЖД")) return _R("МЕЖД"); else
	if (pos == _R("М-ПРЕДК")) return _R("МC-ПРЕДК"); else
	if (pos == _R("РАЗРЫВ_СОЮЗ")) return _R("СОЮЗ"); else
	return "";
};


bool IsMyStemSpecificGrammem(std::string g)
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

std::string Grammem_MyStem2Dialing(std::string pos)
{
	if (pos == "sg") return _R("ед"); else
	if (pos == "pl") return _R("мн"); else
	if (pos == "nom") return _R("им");  else
	if (pos == "gen") return _R("рд");  else
	if (pos == "gen2") return _R("рд,2");  else
	if (pos == "dat") return _R("дт");  else
	if (pos == "acc") return _R("вн");  else
	if (pos == "acc2") return _R("им");  else // "пошел в солдаты"
	if (pos == "ins") return _R("тв");  else
	if (pos == "loc") return _R("пр");  else
	if (pos == "loc2") return _R("пр,2");  else
	if (pos == "m") return _R("мр");  else
	if (pos == "f") return _R("жр");  else
    if (pos == "mf") return _R("мр-жр");  else
	if (pos == "n") return _R("ср");  else

	if (pos == "famn") return _R("фам");  else
	if (pos == "persn") return _R("имя");  else
	if (pos == "patrn") return _R("отч");  else
	if (pos == "imper") return _R("пвл");  else
	if (pos == "imper2") return _R("пвл");  else
	if (pos == "inf") return _R("инф");  else
	if (pos == "partcp") return _R("прч");  else
	if (pos == "ger") return _R("дпр");  else
	if (pos == "1p") return _R("1л");  else
	if (pos == _R("1-л")) return _R("1л");  else
	if (pos == "2p") return _R("2л");  else
	if (pos == "3p") return _R("3л");  else
	if (pos == "comp") return _R("сравн");  else
	if (pos == "comp2") return _R("сравн,2");  else
	if (pos == "supr") return _R("прев");  else
	if (pos == _R("прев")) return _R("прев");  else
	if (pos == "brev") return _R("кр");  else
	if (pos == "sh") return _R("кр");  else
	if (pos == "praes") return _R("нст");  else
	if (pos == "abbr") return _R("аббр");  else
	if (pos == _R("наст*")) return _R("нст");  else
	if (pos == "fut") return _R("буд");  else
	if (pos == "praet") return _R("прш");  else
	if (pos == "act") return _R("дст");  else
	if (pos == "pass") return _R("стр");  else
	if (pos == "anim") return _R("од");  else
	if (pos == "inan") return _R("но");  else
	if (pos == "pf") return _R("св");  else
	if (pos == "ipf") return _R("нс");  else
	if (pos == "tran") return _R("пе");  else
	if (pos == "intr") return _R("нп");  else
	if (pos == "m-f") return _R("мр-жр");  else
	if (pos == _R("мн")) return _R("мн");  else
	if (pos == _R("рд")) return _R("рд");  else
	if (pos == _R("ед")) return _R("ед");  else
	if (pos == _R("но")) return _R("но");  else
	if (pos == _R("дст")) return _R("дст");  else
	if (pos == _R("кр")) return _R("кр");  else
	if (pos == _R("вн")) return _R("вн");  else
	if (pos == _R("тв")) return _R("тв");  else
	if (pos == _R("пр")) return _R("пр");  else
	if (pos == _R("од")) return _R("од");  else
	if (pos == _R("и")) return _R("им");  else

	if (pos == _R("пвл")) return _R("пвл");  else
	
	if (pos == _R("мр")) return _R("мр");  else
	if (pos == _R("жр")) return _R("жр");  else
	if (pos == _R("ср")) return _R("ср");  else
	if (pos == _R("стр")) return _R("стр");  else
	if (pos == _R("пе")) return _R("пе");  else
	if (pos == _R("им")) return _R("им");  else
	if (pos == _R("ср")) return _R("ср");  else
	if (pos == "voc") return _R("зв");  else
	if (pos == "0") return "0";  else
	if (IsMyStemSpecificGrammem(pos)) return pos;  else
	
		
	
	return "";

};


