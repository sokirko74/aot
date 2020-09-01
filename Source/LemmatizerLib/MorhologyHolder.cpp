// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "MorphologyHolder.h"

CMorphologyHolder::CMorphologyHolder()
{
	m_pLemmatizer = 0;
	m_pGramTab = 0;
	m_bTimeStatis = false;
};

CMorphologyHolder::~CMorphologyHolder()
{
	DeleteProcessors();
};

void CMorphologyHolder::DeleteProcessors()
{
	if (m_pLemmatizer) 
	{
		delete m_pLemmatizer;
		m_pLemmatizer = 0;
	}
	if (m_pGramTab)
	{
		delete m_pGramTab;
		m_pGramTab = 0;
	}

};

bool CMorphologyHolder::LoadLemmatizer(MorphLanguageEnum langua)
{
    try {
        DeleteProcessors();
		if (langua == morphRussian)
		{
			m_pGramTab = new CRusGramTab;
			m_pLemmatizer = new CLemmatizerRussian;
		}
		else
			if (langua == morphGerman)
			{
				m_pGramTab = new CGerGramTab;
				m_pLemmatizer = new CLemmatizerGerman;
			}
			else
			if (langua == morphEnglish)
			{
				m_pGramTab = new CEngGramTab;
				m_pLemmatizer = new CLemmatizerEnglish;
			}
			else
			{
				ErrorMessage ("unsupported language");
				return false;
			};
		std::string strError;
		if (!m_pLemmatizer->LoadDictionariesRegistry(strError))
		{
			ErrorMessage(strError);
			return false;
		};
		if (!m_pGramTab->LoadFromRegistry())
		{	
			ErrorMessage("Cannot load gramtab\n");
			return false;
		};

		m_PlmLines.m_pLemmatizer = m_pLemmatizer;
		m_CurrentLanguage = langua;
		return true;
	}
	catch(...)
	{
		return false;
	};

}


bool CMorphologyHolder::LoadGraphanAndLemmatizer(MorphLanguageEnum langua)
{
	try
	{
		DeleteProcessors();

		m_Graphan.FreeDicts();
		m_Graphan.m_Language = langua;
        if (!m_Graphan.LoadDicts())
		{	
			ErrorMessage("Cannot load graphan\n");
			return false;
		}
        if (! LoadLemmatizer(langua) )
            return false;

		return true;
	}
	catch(...)
	{
		return false;
	};

};


bool CMorphologyHolder::GetMorphology(std::string str, bool bFile, int& CountOfWords)
{
	clock_t t1,t2;
	CountOfWords = 0;

	try {
		// ============  Graphematics =======================
		if (m_bTimeStatis) t1= clock();
		if (bFile) {
			m_Graphan.LoadFileToGraphan(str);
		}
		else {
			m_Graphan.LoadStringToGraphan(str);
		}

		if (m_bTimeStatis) 
		{
			t2 = clock();
			size_t TokensCount = m_Graphan.GetTokensCount();
			for (int i = 0; i <TokensCount; i++)
				if (m_Graphan.GetTokenLanguage(i) == m_CurrentLanguage )
							CountOfWords++;
			fprintf (stderr,"CountOfWords = %i\n",CountOfWords);

			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			fprintf(stderr,"Graphan: Ticks = %i Speed = %6.0f\n", t2-t1, speed );
		};

		// ============  Morphology =======================
		if (m_bTimeStatis) t1= clock();

		//m_Graphan.WriteGraphMat("graphan.gra");

		m_pLemmatizer->ProcessHyphenWords(&m_Graphan);

		if (!m_PlmLines.ProcessPlmLines(&m_Graphan))
		{
			fprintf (stderr, "  Cannot get morph. interpretation from Lemmatizer\n");;
			return false;
		};

		if (m_bTimeStatis) 
		{
			t2 = clock();
			double speed =  ((double)CountOfWords)/((t2-t1)/((double)CLOCKS_PER_SEC));
			fprintf(stderr,"Morphology: Ticks = %i Speed = %6.0f\n", t2-t1, speed );
		};
		m_Graphan.FreeTable();
		return true;
	}
	catch (CExpc e) {
		std::cerr << e.m_strCause << "\n";
		return false;	}
	catch (...)
	{
		return false;
	};


};


DwordVector CMorphologyHolder::GetParadigmIdsByNormAndAncode(std::string &str, const std::string &code) const
{
	std::vector<CFormInfo> ParadigmCollection;
	DwordVector res;
	if (!m_pLemmatizer->CreateParadigmCollection(true, str, true, false, ParadigmCollection ))
		throw CExpc(Format("Cannot lemmatize %s", str.c_str()));

	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;
		std::string ancode = Paradigm.GetAncode(0);
		for (long j=0;  j < code.size(); j+=2)
			for (long k=0;  k < ancode.size(); k+=2)
				if(ancode.substr(k, 2) == code.substr(j, 2))
				{
					res.push_back(Paradigm.GetParadigmId());
					goto next_paradigm;
				}
next_paradigm:;
	}
	return res;
}

void CMorphologyHolder::string_to_ids(const char *str, DwordVector &ids, bool bNorm) const 
{
	ids.clear();
	
	std::vector<CFormInfo > ParadigmCollection;
	
	std::string Word = str;
	if (!m_pLemmatizer->CreateParadigmCollection(bNorm, Word, true, false, ParadigmCollection))
	{
		fprintf (stderr,"cannot lemmatize %s\n", str);
		return;
	}
	

	for(int i = 0; i < ParadigmCollection.size(); i++)
	{
		const CFormInfo& Paradigm = ParadigmCollection[i];
		if(!Paradigm.m_bFound) continue;

		ids.push_back(Paradigm.GetParadigmId());
	}
}

std::string CMorphologyHolder::id_to_string(long id) const
{
	
	CFormInfo Res;
	if (!m_pLemmatizer->CreateParadigmFromID(id, Res))
		throw CExpc (Format( "cannot get lemma  by id %i", id));
	return Res.GetWordForm(0);
}

CFormInfo CMorphologyHolder::id_to_paradigm(long id) const
{
	
	CFormInfo Res;
	if (!m_pLemmatizer->CreateParadigmFromID(id, Res))
		throw CExpc (Format( "cannot get lemma  by id %i", id));
	return Res;
}


std::string CMorphologyHolder::GetGrammems(const char* tab_str) {
	QWORD G;
	m_pGramTab->GetGrammems(tab_str, G);
	std::string s = m_pGramTab->GrammemsToStr(G);
	if (!s.empty() && (s[s.length() - 1] == ','))
		s.erase(s.length() - 1);
	return s;
}

std::string CMorphologyHolder::PrintMorphInfoUtf8(std::string Form, bool printIds, bool printForms, bool sortParadigms)
{
	bool bCapital = is_upper_alpha((BYTE)Form[0], m_CurrentLanguage);

	std::vector<CFormInfo> Paradigms;
	m_pLemmatizer->CreateParadigmCollection(false, Form, bCapital, true, Paradigms);

	std::vector<std::string> Results;
	for (int i = 0; i < Paradigms.size(); i++) {
		std::string Result;
		const CFormInfo& F = Paradigms[i];
		Result += F.m_bFound ? "+ " : "- ";

		Result += F.GetWordForm(0) + " ";

		{
			std::string GramCodes = F.GetSrcAncode();
			BYTE PartOfSpeech = m_pGramTab->GetPartOfSpeech(GramCodes.c_str());
			Result += m_pGramTab->GetPartOfSpeechStr(PartOfSpeech) + std::string(" ");

			std::string CommonAncode = F.GetCommonAncode();
			Result += Format("%s ", (CommonAncode.empty()) ? "" : GetGrammems(CommonAncode.c_str()).c_str());

			for (long i = 0; i < GramCodes.length(); i += 2) {
				if (i > 0)
					Result += ";";
				Result += Format("%s", GetGrammems(GramCodes.c_str() + i).c_str());
			}

		}

		if (printIds)
			Result += Format(" %i", F.GetParadigmId());

		BYTE Accent = F.GetSrcAccentedVowel();
		if (Accent != 0xff)
			Result += Format(" %s'%s", Form.substr(0, Accent + 1).c_str(), Form.substr(Accent + 1).c_str());

		if (printForms) {
			Result += " ";
			for (int k = 0; k < F.GetCount(); k++) {
				if (k > 0)
					Result += ",";
				Result += Paradigms[i].GetWordForm(k);
			};
		};
		Results.push_back(Result);
	};

	if (sortParadigms) {
		std::sort(Results.begin(), Results.end());
	};
	std::string Result;
	for (int i = 0; i < Results.size(); i++) {
		if (i > 0)
			Result += "\t";
		Result += Results[i] + "\n";
	}
	return convert_to_utf8(Result, m_CurrentLanguage);
};

