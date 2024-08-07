// ==========  This file is under  LGPL, the GNU Lesser General Public License
// ==========  Dialing Thesaurus Browser (www.aot.ru)
// ==========  Copyright by Alexey Sokirko (2000-2002)

#ifndef __MODELS_H_
#define __MODELS_H_



struct CThesSynRelation {
	int m_Word1;
	int m_Word2;
	int m_Name;
};

struct CAtomGroup {
	BYTE m_PartOfSpeech;
	uint64_t m_Grammems;
	int  m_LanguageId;
    bool m_bDigCmpl;
	CAtomGroup ()
	{
		m_PartOfSpeech = 255;
	    m_Grammems = 0;
		m_LanguageId = 0;
		m_bDigCmpl = false;
	};
};
struct CThesSynGroup {
	std::string m_TypeStr;
	long m_First;
	long m_Last;
};

struct CInnerModel {
	int					 m_ModelId;
	std::vector<CThesSynRelation> m_SynRelations;
	std::vector<CThesSynGroup>	 m_SynGroups;
	std::vector<CAtomGroup>	 m_AtomGroups;
	CInnerModel()
	{
		m_ModelId = -1;
	};
	CInnerModel(int ModedlId)
	{
		m_ModelId = ModedlId;
	};
	bool operator < (const CInnerModel& X) const
	{
		return m_ModelId < X.m_ModelId;
	};
	bool IsRussian() const;
};
#endif


