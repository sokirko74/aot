// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#ifndef MorphDictBuilder_h
#define MorphDictBuilder_h


#include "MorphDict.h"
#include "MorphAutomBuilder.h"

#include "../MorphWizardLib/wizard.h"


class CMorphDictBuilder : public CMorphDict
{


	// m_ModelInfo[i][j] is a word which should be written into CTrieNodeBuild::m_Info
	//  where i is the index of MorphoWizard::m_FlexiaModels
	//  and j is a the index of CFlexiaModel::m_Flexia
	std::vector< std::vector <bool> >		m_ModelInfo;
	std::vector< DwordVector >	m_PrefixSets;

	
	void				ClearRegister();
	
	
	bool				CheckFlexiaGramInfo(const MorphoWizard& Wizard) const;
	void				GeneratePrefixes(const MorphoWizard& Wizard);

	//  debug functions 
	bool				CheckRegister() const;
	
	CMorphAutomatBuilder*	GetFormBuilder() {return (CMorphAutomatBuilder*)m_pFormAutomat; };
	
	
	
public:
	CMorphDictBuilder(MorphLanguageEnum Language);
	~CMorphDictBuilder();

	void	CreateAutomat(const MorphoWizard& Wizard);
	void	GenerateLemmas(const MorphoWizard& Wizard);
	void	GenerateUnitedFlexModels(const MorphoWizard& Wizard);
	bool	GenPredictIdx(const MorphoWizard& wizard, int PostfixLength, int MinFreq, std::string path);
};




#endif
