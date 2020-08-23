// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru), 
// ==========  Copyright by Alexey Sokirko (2004)

#include "StdMorph.h"
#include "MorphDict.h"
#include "LemmaInfoSerialize.h"
#include <fstream>


//======================================================
//=============        CMorphDict               =============
//======================================================


CMorphDict::CMorphDict(MorphLanguageEnum language) : 
    m_SearchInfoLess(m_Bases)
{
    m_pFormAutomat  = 0;
    m_Language = language;
};

CMorphDict::~CMorphDict() 
{
    if (m_pFormAutomat != nullptr)
        delete m_pFormAutomat;
    m_pFormAutomat = 0;
};

void CMorphDict::InitAutomat(CMorphAutomat*    pFormAutomat)
{
    assert (m_pFormAutomat == 0);
    assert (pFormAutomat != 0);
    m_pFormAutomat = pFormAutomat;
};


void    CMorphDict::GetLemmaInfos (const std::string& Text, size_t TextPos, std::vector<CAutomAnnotationInner>& Infos) const 
{
    const size_t TextLength = Text.length();
    size_t Count =     Infos.size();
    
    for (size_t i=0; i<Count; i++)
    {
        const CAutomAnnotationInner&  A = Infos[i];
        const CFlexiaModel& F = m_FlexiaModels[A.m_ModelNo];
        const CMorphForm& M = F.m_Flexia[A.m_ItemNo];
        size_t TextStartPos = TextPos+m_Prefixes[A.m_PrefixNo].length()+M.m_PrefixStr.length();
        //std::string Base = Text.substr(TextStartPos, TextLength-TextStartPos-M.m_FlexiaStr.length());
        std::string Base = m_Prefixes[A.m_PrefixNo]+Text.substr(TextStartPos, TextLength-TextStartPos-M.m_FlexiaStr.length());

        std::vector<CLemmaInfoAndLemma>::const_iterator start = m_LemmaInfos.begin()+m_ModelsIndex[A.m_ModelNo];
        std::vector<CLemmaInfoAndLemma>::const_iterator end = m_LemmaInfos.begin()+m_ModelsIndex[A.m_ModelNo+1];

        std::vector<CLemmaInfoAndLemma>::const_iterator it =  
            lower_bound (start,  end, Base.c_str(), m_SearchInfoLess);
        
    
        assert (it != m_LemmaInfos.end());
        {
            int LemmaStrNo = it->m_LemmaStrNo;
            assert (Base == m_Bases[LemmaStrNo].GetString());
        }

        Infos[i].m_LemmaInfoNo = it-m_LemmaInfos.begin();

    };

};



void    CMorphDict::PredictBySuffix (const std::string& Text, size_t& TextPos, size_t MinimalPredictSuffixlen, std::vector<CAutomAnnotationInner>& Infos) const 
{
    size_t Count = Text.length();
    
    for (TextPos=1; TextPos+MinimalPredictSuffixlen<=Count; TextPos++)
    {
        m_pFormAutomat->GetInnerMorphInfos(Text, TextPos, Infos);
        if (!Infos.empty()) break;
    };
    
};




inline size_t get_size_in_bytes (const CLemmaInfoAndLemma& t)
{
    return        get_size_in_bytes(t.m_LemmaInfo) 
        +    get_size_in_bytes(t.m_LemmaStrNo);
};

inline size_t save_to_bytes(const CLemmaInfoAndLemma& t, BYTE* buf)
{
    buf += save_to_bytes(t.m_LemmaInfo, buf);
    buf += save_to_bytes(t.m_LemmaStrNo, buf);
    return get_size_in_bytes(t);
};

inline size_t restore_from_bytes(CLemmaInfoAndLemma& t, const BYTE* buf)
{
    buf += restore_from_bytes(t.m_LemmaInfo, buf);
    buf += restore_from_bytes(t.m_LemmaStrNo, buf);
    return get_size_in_bytes(t);
};


// This procedure builds CMorphDict::m_ModelsIndex, which is an index to CMorphDict::m_LemmaInfos
// *  m_LemmaInfos is sorted by m_LemmaInfo.m_FlexiaModelNo
// *  CMorphDict::m_ModelsIndex stores all periods of  CMorphDict::m_ModelsIndex with equal  m_LemmaInfo.m_FlexiaModelNo
// *  if a=m_ModelsIndex[i] and b=m_ModelsIndex[i+1], then for each j (a<=j<b)
//     LemmaInfos[j].m_LemmaInfo.m_FlexiaModelNo == i
// *  for some i m_ModelsIndex[i] can be equal to m_ModelsIndex[i+1], it means
//    that flexia model i is not used. To delete unused models the dictionary should be packed.
void CMorphDict::CreateModelsIndex()
{
    m_ModelsIndex.clear();
    if (m_LemmaInfos.empty()) return;

    m_ModelsIndex.resize(m_FlexiaModels.size() + 1);

    int CurrentModel  = m_LemmaInfos[0].m_LemmaInfo.m_FlexiaModelNo;
    m_ModelsIndex[CurrentModel] = 0;

    for (size_t i=0; i< m_LemmaInfos.size(); i++)
            for (; CurrentModel < m_LemmaInfos[i].m_LemmaInfo.m_FlexiaModelNo; CurrentModel++ )
            {
                //int debug = m_LemmaInfos[i].m_LemmaInfo.m_FlexiaModelNo;
                m_ModelsIndex[CurrentModel+1] = i;
            };
    
    for (; CurrentModel < m_FlexiaModels.size(); CurrentModel++ )
        m_ModelsIndex[CurrentModel+1] = m_LemmaInfos.size();
#ifdef DEBUG    
    for (size_t i=0; i< m_LemmaInfos.size(); i++)
    {
        int debug = m_LemmaInfos[i].m_LemmaInfo.m_FlexiaModelNo;
        assert (m_ModelsIndex[debug] <= i);
        assert (i < m_ModelsIndex[debug+1]);
    };
#endif
};

static size_t getCount(std::ifstream& mrdFile, const char* sectionName) {
    std::string line;
    if (!getline(mrdFile, line)) {
        throw CExpc("Cannot get size of section  %s", sectionName);
    }
    return atoi( line.c_str() );
}

bool CMorphDict::Load(std::string GrammarFileName)
{
    //fprintf (stderr," open %s\n", GrammarFileName.c_str());
    if (!m_pFormAutomat->Load(MakeFName(GrammarFileName,"forms_autom")))
        return false;

    std::string PrecompiledFile = MakeFName(GrammarFileName,"annot");
    std::ifstream annotFile(PrecompiledFile, std::ios::binary);
    if (!annotFile.is_open() )
    {
        ErrorMessage (Format("Cannot open %s", PrecompiledFile.c_str()));
        return false;
    };
    ReadFlexiaModels(annotFile);
    ReadAccentModels(annotFile);

    {
       size_t count = getCount(annotFile, "prefix sets");
       m_Prefixes.resize(1, "");
       for (size_t num = 0; num < count; num++)
       {
           std::string q;
           if (!getline(annotFile, q)) return false;
           Trim(q);
           assert (!q.empty());
           m_Prefixes.push_back(q);
        };
    }

    {
        size_t count = getCount(annotFile, "lemma infos");
        m_LemmaInfos.clear();
        ReadVectorInner(annotFile, m_LemmaInfos, count);
    }

    {
        size_t count = getCount(annotFile, "nps infos");
        m_NPSs.clear();
        ReadVectorInner(annotFile, m_NPSs, count);
        assert (m_NPSs.size()  == m_FlexiaModels.size());
    }

    m_Bases.ReadShortStringHolder(MakeFName(GrammarFileName,"bases"));

    CreateModelsIndex();

    return true;
};

bool CMorphDict::Save(std::string GrammarFileName) const
{
    try {
        if (!m_pFormAutomat->Save(MakeFName(GrammarFileName,"forms_autom")))
        {
            ErrorMessage (Format("Cannot write to %s", MakeFName(GrammarFileName,"forms_autom").c_str()));
            return false;
        }


        std::string PrecompiledFile = MakeFName(GrammarFileName,"annot");
        std::ofstream outp(PrecompiledFile, std::ios::binary);
        if (!outp.is_open())
        {
            ErrorMessage (Format("Cannot write to %s", PrecompiledFile.c_str()));
            return false;
        };
        WriteFlexiaModels(outp);
        WriteAccentModels(outp);
        assert (!m_Prefixes.empty() && m_Prefixes[0].empty());
        
        // do not write the first empty prefix, instead add it manually each time during loading
        outp << m_Prefixes.size() - 1 << "\n";
        for (size_t i = 1; i < m_Prefixes.size(); i++) {
            outp << m_Prefixes[i] << "\n";
        }
        
        outp << m_LemmaInfos.size() << "\n";
        if (!WriteVectorStream(outp, m_LemmaInfos)) return false;

        assert (m_NPSs.size()  == m_FlexiaModels.size());
        outp <<  m_NPSs.size() << "\n";
        if (!WriteVectorStream(outp, m_NPSs)) return false;
        outp.close();

        if (!m_Bases.WriteShortStringHolder(MakeFName(GrammarFileName,"bases")))
        {
            fprintf(stderr, "Cannot save bases\n");
            return false;
        };

        return true;
    }
    catch (...)
    {
        fprintf (stderr, "Cannot save CMorphDict");
        return false;
    };
};



