// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)


#include "StdRmlThes.h"
#include "Thesaurus.h"

/////////////////////////////////////////////////////////////////////////////
// CThesaurus
template<class _II, class _Ty> inline
bool _find(_II It, const _Ty& _V)
{
	return !(find(It.begin(), It.end(), _V) == It.end());
}


CThesaurus::CThesaurus()
{
	m_pOborDictionary = 0;
	m_pMainGramTab = 0;
	m_pEngGramTab = 0;
	m_MainLanguage = morphUnknown;
}

CThesaurus::~CThesaurus()
{

}

bool CThesaurus::ReadThesaurusFromDisk()
{
	//ErrorMessage ("Cannot Load models " +m_Directory+"/StatThes/FreqCollocTypes.txt");
	if (m_bDontLoad) return true;

	if (!m_pMainGramTab) return false;
	if (!m_pEngGramTab) return false;
	if (m_MainLanguage == morphUnknown) return false;

	if (!LoadModels(m_Directory + "/StatThes/FreqCollocTypes.txt"))
	{
		ErrorMessage("Cannot Load models " + m_Directory + "/StatThes/FreqCollocTypes.txt");
		return false;
	};

	std::string OborotsFileName = (m_MainLanguage == morphGerman) ?
		m_Directory + "/StatThes/GerOborots.txt" : m_Directory + "/StatThes/RusOborots.txt";
	if (!LoadOborots(OborotsFileName))
	{
		ErrorMessage("Cannot Load oborots " + OborotsFileName);
		return false;

	};


	if (!LoadTermins(m_Directory + "/StatThes/TextEntr.txt"))
	{
		ErrorMessage("Cannot Load text entries " + m_Directory + "/StatThes/TextEntr.txt");
		return false;

	};

	if (!LoadSynItems(m_Directory + "/StatThes/Lemmas.txt"))
	{
		ErrorMessage("Cannot Load lemmas " + m_Directory + "/StatThes/Lemmas.txt");
		return false;

	};

	if (!LoadSynonyms(m_Directory + "/StatThes/Synonyms.txt"))
	{
		ErrorMessage("Cannot Load text entries " + m_Directory + "/StatThes/Synonyms.txt");
		return false;

	};


	return true;
}




bool CThesaurus::ReadRelationsFromDisk()
{
	if (m_bDontLoad) return true;

	if (!LoadConcepts(m_Directory + "/StatThes/Concepts.txt"))
	{
		ErrorMessage("Cannot Load Concepts " + m_Directory + "/StatThes/Concepts.txt");
		return false;

	};
	if (!LoadRelats(m_Directory + "/StatThes/Relats.txt"))
	{
		ErrorMessage("Cannot Load Relats " + m_Directory + "/StatThes/Relats.txt");
		return false;

	};
	return true;
}

bool CThesaurus::SetDatabase(std::string DatabaseName)
{
	m_bDontLoad = false;
	m_Name = DatabaseName;
	std::string KeyName = "Software\\Dialing\\" + m_Name + "\\DictPath";
	try {
		m_Directory = GetRegistryString(KeyName);
	}
	catch (...)
	{
		ErrorMessage("Cannot open" + m_Name);
		return false;
	};

	try {
		std::string KeyName = "Software\\Dialing\\Thesauri\\IgnoreThesauri";
		if (CanGetRegistryString(KeyName))
		{
			std::string Value = GetRegistryString(KeyName);
			m_bDontLoad = (Value == "yes");
		}
	}
	catch (...)
	{
	};

	return true;
}




struct IsLessByConcept1 {

	bool  operator() (const CRelat& _X1, const thesaurus_concept_t& conceptId) const
	{
		return _X1.m_Concept1Id < conceptId;
	};
	bool  operator() (const thesaurus_concept_t& conceptId, const CRelat& _X1) const
	{
		return conceptId < _X1.m_Concept1Id;
	};
	bool  operator() (const CRelat& _X1, const CRelat& _X2) const
	{
		return _X1.m_Concept1Id < _X2.m_Concept1Id;
	};
};



static size_t GetRelationsCount(const CThesaurus& T, thesaurus_concept_t conceptId, RelationEnum relationId)
{
	auto p = std::equal_range(T.m_Relats.begin(), T.m_Relats.end(), conceptId, IsLessByConcept1());
	size_t result = 0;
	for (auto i = p.first; i != p.second; ++i)
	{
		if (i->m_RelationType == relationId) {
			++result;
		}
	}
	return result;
};


void CThesaurus::dfs(thesaurus_concept_t conceptId, RelationEnum relationType, std::unordered_set<thesaurus_concept_t>& concepts) const
{
	if (concepts.find(conceptId) != concepts.end()) {
		return; 
	}
	concepts.insert(conceptId);
	auto p = std::equal_range(m_Relats.begin(), m_Relats.end(), conceptId, IsLessByConcept1());
	for (auto i = p.first; i != p.second; ++i)
	{
		if (i->m_RelationType == relationType)
		{
			dfs(i->m_Concept2Id,  relationType, concepts);
		};

	}
};


bool CThesaurus::IsA(DWORD TextEntryId, std::string ConceptStr) const
{
	thesaurus_concept_t conceptId = GetConceptIdByConceptStr(ConceptStr.c_str());
	if (conceptId == UnknownConceptId) return   false;

	std::vector<thesaurus_concept_t> conceptsFromTextEntry;
	GetConceptsByTextEntryId(TextEntryId, conceptsFromTextEntry);
	if (conceptsFromTextEntry.empty()) return true;

	for (auto& concept : conceptsFromTextEntry)
	{
		std::unordered_set<thesaurus_concept_t> higherConcepts;
		dfs(concept, HIGHER, higherConcepts);
		if (higherConcepts.find(conceptId) != higherConcepts.end()) {
			return true;
		}
	};

	return false;
}

bool IsEnglishModel(const CInnerModel& M)
{
	for (long i = 0; i < M.m_AtomGroups.size(); i++)
		if (M.m_AtomGroups[i].m_LanguageId != morphEnglish)
			return false;

	return true;

};

void CThesaurus::QueryEnglishTranslations(DWORD TextEntryId, std::vector<int>& CurrentEnglishTermins) const
{
	CurrentEnglishTermins.clear();
	std::vector<thesaurus_concept_t> ConceptsFromTextEntry;
	GetConceptsByTextEntryId(TextEntryId, ConceptsFromTextEntry);
	if (ConceptsFromTextEntry.empty()) return;

	for (long i = 0; i < m_SynonymsByConcept.size(); i++)
	{
		bool Found = false;
		long k = 0;
		for (; k < ConceptsFromTextEntry.size(); k++)
			if (m_SynonymsByConcept[i].m_ConceptId == ConceptsFromTextEntry[k])
			{
				Found = true;
				break;
			};

		if (Found)
		{
			k = GetTerminNoByTextEntryId(m_SynonymsByConcept[i].m_TextEntryId);
			if (k != -1)
			{
				if (m_Termins[k].m_ModelNo != -1)
					if (IsEnglishModel(m_Models[m_Termins[k].m_ModelNo]))
						CurrentEnglishTermins.push_back(k);

				std::string s = m_Termins[k].m_TerminStr;
				long j = 0;
				for (; j < s.length(); j++)
					if (!is_english_alpha((BYTE)s[j])
						&& !isdigit((BYTE)s[j])
						&& !ispunct((BYTE)s[j])
						&& !isspace((BYTE)s[j])
						)
						break;

				if (j == s.length())
					if (!_find(CurrentEnglishTermins, k))
						CurrentEnglishTermins.push_back(k);

			};
		};
	}

}



int CThesaurus::GetTerminIdBySingleWord(std::string WordStr) const
{
	CInnerSynItem I;
	I.m_ItemStr = WordStr;

	EngRusMakeUpper(I.m_ItemStr);

	for (std::vector<CInnerSynItem>::const_iterator It = lower_bound(m_SynItems.begin(), m_SynItems.end(), I);
		(It < m_SynItems.end()) && (It->m_ItemStr == I.m_ItemStr);
		It++)
		if (m_Termins[It->m_TerminNo].m_ModelNo != -1)
			if (m_Models[m_Termins[It->m_TerminNo].m_ModelNo].m_AtomGroups.size() == 1)
			{
				return  m_Termins[It->m_TerminNo].m_TerminId;

			};

	return -1;
}

thesaurus_concept_set_t CThesaurus::QueryTopConcepts(UINT TextEntryId) const
{
	std::vector<thesaurus_concept_t> conceptsFromTextEntry;
	GetConceptsByTextEntryId(TextEntryId, conceptsFromTextEntry);

	thesaurus_concept_set_t result;
	for (auto i: conceptsFromTextEntry)
	{
		thesaurus_concept_set_t higherConcepts;
		dfs(i, HIGHER, higherConcepts);
		for (auto k : higherConcepts) {
			if (GetRelationsCount(*this, k, HIGHER) == 0) {
				result.insert(k);
			}
		}
	};
	return result;
}



DWORD CThesaurus::QueryTerminItem(const std::string& ItemStr, std::vector<int>& CurrentTerminItems) const
{
	CurrentTerminItems.clear();
	CInnerSynItem I;
	I.m_ItemStr = ItemStr;
	for (std::vector<CInnerSynItem>::const_iterator It = lower_bound(m_SynItems.begin(), m_SynItems.end(), I);
		(It < m_SynItems.end()) && (It->m_ItemStr == I.m_ItemStr);
		It++)
		CurrentTerminItems.push_back(It - m_SynItems.begin());

	return  CurrentTerminItems.size();
}



thesaurus_termin_set_t CThesaurus::QueryLowerTermins(const std::string& conceptStr, MorphLanguageEnum lang) const
{
	thesaurus_termin_set_t result;
	thesaurus_concept_t conceptId = GetConceptIdByConceptStr(conceptStr);
	thesaurus_concept_set_t lowerConcepts;
	dfs(conceptId, LOWER, lowerConcepts);

	bool IsEnglish = lang == morphEnglish;
	for (auto i : lowerConcepts)
	{
		std::vector<long> TextEntries;
		GetTextEntriesByConcept(i, TextEntries);
		for (auto k : TextEntries)
		{
			thesaurus_termin_t TerminNo = GetTerminNoByTextEntryId(k);
			if (m_Termins[TerminNo].m_ModelNo == -1) continue;
			if (IsEnglish != IsEnglishModel(m_Models[m_Termins[TerminNo].m_ModelNo])) continue;
			result.insert(TerminNo);
		};
	};
	return result;
}


int CThesaurus::FindAbbr(const std::string& str) const
{
	String2Int::const_iterator It = m_AbbrForms2TerminNo.find(str);
	if (It != m_AbbrForms2TerminNo.end())
		return  It->second;
	else
		return  -1;
}


bool CThesaurus::LoadSynItems(std::string FileName)
{
	m_SynItems.clear();
	FILE* fp = fopen(FileName.c_str(), "r");
	if (!fp) return false;
	char buff[2000];
	if (!fgets(buff, 2000, fp))
		return false;
	std::string Header = buff;
	Trim(Header);
	if (Header != "TextEntryId;LemmaPos;LemmaStr;IsLemma;IsOb1;IsOb2;OborotId;UpLw;UpUp;IsPlural;")
	{
		fclose(fp);
		return false;
	}

	long SaveTerminId = -1;
	long SaveTerminNo = -1;
	while (fgets(buff, 2000, fp))
	{
		int i = 0;
		CInnerSynItem I;
		long TerminId;
		long OborotId = -1;
		for (char* s = strtok(buff, FieldDelimiter); s; s = strtok(0, FieldDelimiter))
		{
			int len = strlen(s);
			if (s[0] == '"')
			{
				if (s[len - 1] != '"')
				{
					fclose(fp);
					return false;
				}
				s[len - 1] = ' ';
				s++;

			};
			std::string Field = s;
			if (i == 0)
				TerminId = atoi(Field.c_str());
			else
				if (i == 1)
					I.m_ItemPos = atoi(Field.c_str());
				else
					if (i == 2)
					{
						I.m_ItemStr = convert_from_utf8(s, m_MainLanguage);
						Trim(I.m_ItemStr);
					}
					else
						if (i == 3)
							I.m_Flags |= (Field == "TRUE") ? siLemma : 0;
						else
							if (i == 4)
								I.m_Flags |= (Field == "TRUE") ? siOb1 : 0;
							else
								if (i == 5)
									I.m_Flags |= (Field == "TRUE") ? siOb2 : 0;
								else
									if (i == 6)
										OborotId = atoi(Field.c_str());
									else
										if (i == 7)
											I.m_Flags |= (Field == "TRUE") ? siUpLw : 0;
										else
											if (i == 8)
												I.m_Flags |= (Field == "TRUE") ? siUpUp : 0;
											else
												if (i == 9)
													I.m_Flags |= (Field == "TRUE") ? siPlural : 0;

			i++;
		}
		I.m_OborotNo = ErrUnitNo;
		if (OborotId != -1)
		{
			std::vector<COborot>::const_iterator It = lower_bound(m_Oborots.begin(), m_Oborots.end(), COborot(OborotId));
			if ((It != m_Oborots.end())
				&& (It->m_OborotId == OborotId)
				)
				I.m_OborotNo = It - m_Oborots.begin();
		};
		long TerminNo;
		if (TerminId == SaveTerminId)
			TerminNo = SaveTerminNo;
		else
			TerminNo = GetTerminNoByTextEntryId(TerminId);

		if (TerminNo != -1)
		{
			I.m_TerminNo = TerminNo;
			m_SynItems.push_back(I);
			SaveTerminNo = I.m_TerminNo;
			SaveTerminId = TerminId;
		};
	};
	sort(m_SynItems.begin(), m_SynItems.end());

	long Count = m_SynItems.size();
	long i = 0;
	try {
		for (i = 0; i < Count; i++)
		{
			CInnerTermin& T = m_Termins[m_SynItems[i].m_TerminNo];
			long k = 0;
			for (; k < T.m_TermItems.size(); k++)
				if (m_SynItems[T.m_TermItems[k]].m_ItemPos > m_SynItems[i].m_ItemPos)
					break;
			T.m_TermItems.insert(T.m_TermItems.begin() + k, i);
		};
	}
	catch (...)
	{
		return false;
	}
	fclose(fp);
	return true;
};


void	CThesaurus::SetDicts(MorphLanguageEnum MainLanguage, const CDictionary* OborDic, const CAgramtab* MainGramtab, const CAgramtab* EngGramtab)
{
	m_MainLanguage = MainLanguage;
	m_pOborDictionary = OborDic;
	m_pMainGramTab = MainGramtab;
	m_pEngGramTab = EngGramtab;
}

