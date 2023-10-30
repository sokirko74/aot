// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "stdafx.h"
#include "ThesaurusForSyntax.h"
#include "SyntaxInit.h"
#include "dicts/RmlThesLib/RmlThesConsts.h"
#include "morph_dict/agramtab/EngGramTab.h"

EThesType GetThesTypeByStr(const std::string& strDBName)
{
	if (strDBName == "RML_THES_LOC") return LocThes;
	if (strDBName == "RML_THES_FIN") return FinThes;
	if (strDBName == "RML_THES_OMNI") return OmniThes;
	if (strDBName == "RML_THES_COMP") return CompThes;
	return	NoneThes;
}

//=======================
CTermin::CTermin()
{
	m_pModel = 0;
};

bool CTermin::operator<(const CTermin& term) const
{
	assert(size() > 0);
	return strcmp((*this)[0], term[0]) < 0;
}

std::string CTermin::GetTerminStr() const
{
	std::string res;
	for (int i = 0; i < size(); i++)
	{
		res += (*this)[i];
		res += " ";
	}
	return res;

}

//========================

CThesaurusForSyntax::CThesaurusForSyntax(const CSyntaxOpt* Opt)
{
	m_pSyntaxOptions = Opt;
	m_pEngGramTab = new CEngGramTab;
	m_pEngGramTab->LoadFromRegistry();
};

CThesaurusForSyntax::~CThesaurusForSyntax()
{
	if (m_pEngGramTab)
		delete m_pEngGramTab;
};

CThesaurus* CThesaurusForSyntax::LoadThesaurus(std::string ThesName) const
{
	CThesaurus* T = new CThesaurus;
	if (!T) return 0;
	if (!T->SetDatabase(ThesName)) return 0;
	assert(GetOpt()->GetOborDictionary() != 0);
	T->m_MainLanguage = GetOpt()->m_Language;
	T->m_pOborDictionary = GetOpt()->GetOborDictionary();
	assert(GetOpt()->GetGramTab() != 0);
	T->m_pMainGramTab = GetOpt()->GetGramTab();
	assert(m_pEngGramTab != 0);
	T->m_pEngGramTab = m_pEngGramTab;
	if (!T->ReadThesaurusFromDisk()) return 0;
	if (!T->ReadRelationsFromDisk()) return 0;

	return T;
};

const CTermin& CThesaurusForSyntax::GetTermin(int i) const
{
	return m_Termins[i];
}

const CVectorOfTermins* CThesaurusForSyntax::GetTermins() const
{
	return &m_Termins;
}


void CThesaurusForSyntax::ReadThesaurusForSyntax(std::string strDBName, const CThesaurus* Thes)
{
	EThesType eThesType;
	eThesType = GetThesTypeByStr(strDBName);
	if (eThesType == NoneThes)
		throw CExpc (Format("%s is unknown thesaurus", strDBName.c_str()));
	ReadModels(*Thes, eThesType);
	ReadTermins(Thes, eThesType);
}


void CThesaurusForSyntax::SortIndexes()
{
	CTerminSort_less pred(&m_Termins);

	for (CInt2Vector::iterator iter = m_TerminsGrouppedByLength.begin();
		iter != m_TerminsGrouppedByLength.end();
		iter++)
	{
		CIntVector& IndexVector = iter->second;
		sort(IndexVector.begin(), IndexVector.end(), pred);
	}
};

bool CThesaurusForSyntax::ReadOneTermin(const CThesaurus* piThes, const CInnerTermin& inputTerm, CTermin& outTerm) const {
	if (inputTerm.m_TermItems.empty()) {
		return false;
	}
	for (const auto& i : inputTerm.m_TermItems)
	{
		const CInnerSynItem& piItem = piThes->m_SynItems[i];
		if (piItem.m_ItemStr.empty() || piItem.m_ItemStr == "-")
		{
			return false;
		}
		outTerm.push_back(piItem.m_ItemStr.c_str());
	};

	outTerm.m_TerminId = inputTerm.m_TerminId;
	outTerm.m_strTermin = inputTerm.m_TerminStr.c_str();
	return true;
}

void CThesaurusForSyntax::ReadTermins(const CThesaurus* piThes, EThesType eThesType)
{
	const std::vector<CGroups>& Models = m_AllThesModels.find(eThesType)->second;
	int termId = (int)m_Termins.size();

	for (const auto& inputTerm : piThes->m_Termins) {
		if ((inputTerm.m_ModelNo < 0) || inputTerm.m_ModelNo >= piThes->m_Models.size()) {
			continue;
		}
		const auto& model = piThes->m_Models[inputTerm.m_ModelNo];
		if (!model.IsRussian()) {
			continue;
		}
		CTermin termin;
		termin.m_pModel = &(Models[inputTerm.m_ModelNo]);
		termin.m_ThesType = eThesType;

		if (ReadOneTermin(piThes, inputTerm, termin)) {
			m_Termins.push_back(termin);
			auto iter = m_TerminsGrouppedByLength.find(inputTerm.m_TermItems.size());
			if (iter == m_TerminsGrouppedByLength.end())
			{
				m_TerminsGrouppedByLength[inputTerm.m_TermItems.size()] = { termId };
			}
			else
			{
				iter->second.push_back(termId);
			}
			++termId;
		}
	}
}


bool InitWordScheme(const CSyntaxOpt* Opt, CSynPlmLine& WordScheme, const CAtomGroup& piWordScheme)
{
	return true;
}

void CThesaurusForSyntax::ReadModels(const CThesaurus& Thes, EThesType eThesType)
{
	size_t ModelNo = 0;
	std::string Name = Thes.m_Name;
	try {
		std::vector<CGroups>& Models = m_AllThesModels[eThesType];

		long iModelsCount = Thes.m_Models.size();
		//printf ("iModelsCount = %i\n",iModelsCount);

		for (; ModelNo < iModelsCount; ModelNo++)
		{
			const CInnerModel& piModel = Thes.m_Models[ModelNo];
			CGroups model(GetOpt());
			long DatabaseModelId = piModel.m_ModelId;

			long iAtomGroupsCount = piModel.m_AtomGroups.size();
			bool bRus = false;
			int j = 0;
			for (; j < iAtomGroupsCount; j++)
			{
				const CAtomGroup& ThesWordScheme = piModel.m_AtomGroups[j];

				CSynPlmLine WordScheme;
				WordScheme.SetGrammems(ThesWordScheme.m_Grammems);
				WordScheme.SetPoses((1 << ThesWordScheme.m_PartOfSpeech));
				if (ThesWordScheme.m_LanguageId == morphRussian)
					WordScheme.SetFlag(fl_is_russian);
				if (ThesWordScheme.m_bDigCmpl) WordScheme.SetFlag(fl_dg_ch);
				if (GetOpt()->GetGramTab()->IsMorphNoun(WordScheme.GetPoses())) WordScheme.SetFlag(fl_morph_noun);
				bRus = bRus || WordScheme.HasFlag(fl_is_russian);
				model.AddWord(WordScheme);
			}



			if (bRus)
			{
				long iGroupsCount = piModel.m_SynGroups.size();


				for (j = 0; j < iGroupsCount; j++)
				{
					const CThesSynGroup& piGroup = piModel.m_SynGroups[j];
					assert(piGroup.m_First > 0);
					assert(piGroup.m_Last > 0);
					CGroup group(piGroup.m_First - 1, piGroup.m_Last - 1, 0);
					group.m_GroupType = GetOpt()->GetGroupTypebyName(piGroup.m_TypeStr.c_str());
					//  by default the main word is the first fort, later it will be changed
					group.m_MainWordNo = group.m_iFirstWord;
					model.create_group(group);
				}

				//printf ("ModelId %i, iGroupsCount %i \n",piModel.m_ModelId, iGroupsCount);

				AssignMainGroupsToModel(model, piModel);
			}

			Models.push_back(model);

		}

	}
	catch (...)
	{
		throw CExpc(Format("Cannot read models for %s ModelNo=%i", Thes.m_Name.c_str(), ModelNo));
	};
}

