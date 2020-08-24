// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "stdafx.h"
#include "ThesaurusForSyntax.h"
#include "SyntaxInit.h"
#include "../RmlThesLib/RmlThesConsts.h"
#include "../AgramtabLib/EngGramTab.h"



CThesaurusForSyntax::CThesaurusForSyntax(const CSyntaxOpt* Opt)
{ 
	m_pSyntaxOptions = Opt;
	m_pEngGramTab  = new CEngGramTab;
	m_pEngGramTab->LoadFromRegistry();
};

CThesaurusForSyntax::~CThesaurusForSyntax() 
{ 
	if (m_pEngGramTab) 
		delete m_pEngGramTab;
};

CThesaurus* CThesaurusForSyntax::LoadThesaurus(const char* ThesName) const
{
	CThesaurus* T = new CThesaurus;
	if (!T) return 0;
	if (!T->SetDatabase(ThesName)) return 0;
	assert (GetOpt()->GetOborDictionary() != 0);
	T->m_MainLanguage = GetOpt()->m_Language;
	T->m_pOborDictionary = GetOpt()->GetOborDictionary();
	assert (GetOpt()->GetGramTab() != 0);
	T->m_pMainGramTab = GetOpt()->GetGramTab();
	assert (m_pEngGramTab != 0);
	T->m_pEngGramTab = m_pEngGramTab;
	if (!T->ReadThesaurusFromDisk()) return 0;
	if (!T->ReadRelationsFromDisk()) return 0;

	return T;
};

bool CThesaurusForSyntax::ReadThesaurusForSyntax(const char* strDBName,  const CThesaurus* Thes, StringHashSet& p_vectorAccost)
{
	try
	{

		
		EThesType eThesType;
		eThesType = GetThesTypeByStr(strDBName);
		if( eThesType == NoneThes)
			return false;

		

if (!strcmp(strDBName, "RML_THES_OMNI"))
{
	try
	{
		std::string s_accost = "PROF";
		std::vector<int> Res;
		Thes->QueryLowerTermins(s_accost.c_str(), morphRussian, Res);
		for (int i = 0; i < Res.size(); i++)
		{
			std::string TerminStr = Thes->m_Termins[Res[i]].m_TerminStr;
			RmlMakeLower(TerminStr, GetOpt()->m_Language);
			p_vectorAccost.insert(TerminStr);
		};
	}
	catch (...)
	{
		ErrorMessage("SynAn", "Probably, RMLTHESLib::QueryLowerTermins function failed!");
	}
}
//nim

bool bRes;


bRes = ReadModels(*Thes, eThesType);
if (!bRes)
{
	ErrorMessage(Format("Cannot read models for %s", Thes->m_Name.c_str()));
	return false;
};

bRes = ReadTermins(Thes, eThesType);

return bRes;
	}
	catch (...)
	{
		return false;
	}
	return false;
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

bool CThesaurusForSyntax::ReadTermins(const CThesaurus* piThes, EThesType eThesType)
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
	return true;
}


bool InitWordScheme(const CSyntaxOpt* Opt, CSynPlmLine& WordScheme,	const CAtomGroup& piWordScheme) 
{
	return true;
}

bool CThesaurusForSyntax::ReadModels(const CThesaurus& Thes, EThesType eThesType)
{
	

	size_t ModelNo = 0;
	std::string Name = Thes.m_Name;
	try {
		std::vector<CGroups>& Models = m_AllThesModels[eThesType];

		long iModelsCount = Thes.m_Models.size();
		//printf ("iModelsCount = %i\n",iModelsCount);

		for(; ModelNo < iModelsCount ; ModelNo++ )
		{
			const CInnerModel& piModel = Thes.m_Models[ModelNo];
			CGroups model(GetOpt());
			long DatabaseModelId = piModel.m_ModelId;

			long iAtomGroupsCount = piModel.m_AtomGroups.size();
			bool bRus = false;
			int j = 0 ;
            for(; j < iAtomGroupsCount ; j++ )
            {
                const CAtomGroup& ThesWordScheme = piModel.m_AtomGroups[j];

                CSynPlmLine WordScheme;
                WordScheme.SetGrammems (ThesWordScheme.m_Grammems);
                WordScheme.SetPoses((1 << ThesWordScheme.m_PartOfSpeech));
                if (ThesWordScheme.m_LanguageId == morphRussian)
                    WordScheme.SetFlag(fl_is_russian);
                if (ThesWordScheme.m_bDigCmpl) WordScheme.SetFlag(fl_dg_ch);
                if (GetOpt()->GetGramTab()->IsMorphNoun(WordScheme.GetPoses())) WordScheme.SetFlag(fl_morph_noun);
                bRus = bRus || WordScheme.HasFlag(fl_is_russian);
                model.AddWord(WordScheme);		
            }

			

			if( bRus )
			{
				long iGroupsCount = piModel.m_SynGroups.size();
				
				
				for(j = 0 ; j < iGroupsCount ; j++ )
				{
					const CThesSynGroup& piGroup = piModel.m_SynGroups[j];		
					assert (piGroup.m_First > 0);
					assert (piGroup.m_Last > 0);
					CGroup group (piGroup.m_First-1, piGroup.m_Last-1, 0);
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
		ErrorMessage ("Synan", Format ("Cannot read models for %s ModelNo=%i", Thes.m_Name.c_str(), ModelNo));
		return false;
	};
	return true;
}

