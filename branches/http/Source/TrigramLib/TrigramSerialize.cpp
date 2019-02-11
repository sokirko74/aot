// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "TrigramModel.h"
#include "../common/bserialize.h"

inline size_t get_size_in_bytes (const float& t)
{
	return sizeof(t);
};
inline size_t save_to_bytes(const float& i, BYTE* buf)
{
	*((float*)buf) = i;
	return get_size_in_bytes(i);
}

inline size_t restore_from_bytes(float& i, const BYTE* buf)
{
	i = *((float*)buf);
	return get_size_in_bytes(i);
};



inline size_t get_size_in_bytes (const CLambdas& t)
{
	return sizeof(t.m_lambda);
};

inline size_t save_to_bytes(const CLambdas& t, BYTE* buf)
{
	buf += save_to_bytes(t.m_lambda[0], buf);
	buf += save_to_bytes(t.m_lambda[1], buf);
	buf += save_to_bytes(t.m_lambda[2], buf);
	return get_size_in_bytes(t);
}

inline size_t restore_from_bytes(CLambdas& t, const BYTE* buf)
{
	buf += restore_from_bytes(t.m_lambda[0], buf);
	buf += restore_from_bytes(t.m_lambda[1], buf);
	buf += restore_from_bytes(t.m_lambda[2], buf);
	return get_size_in_bytes(t);
};

bool CTrigramModel::SaveBinary()
{
    string Path = GetPathByFile(m_DictionaryFile);
    {
        string filename = Path + "/regtags.txt";
        FILE* fp = fopen(filename.c_str(),"w");
        if (!fp) throw CExpc("cannot write %s", filename.c_str());
        for (size_t i=0; i< m_RegisteredTags.size(); i++)
            fprintf (fp, "%s\n",m_RegisteredTags[i].c_str());
        fclose (fp);
    }
    if (!WriteVector(Path+"/unigrams.bin", m_Unigrams))
        throw CExpc ("cannot write unigrams.bin");
    if (!WriteVector(Path+"/ord_tags.bin", m_TagsOrderedByUnigrams))
        throw CExpc ("cannot write ord_tags.bin");
    if (!WriteVector(Path+"/bigrams.bin", m_Bigrams))
        throw CExpc ("cannot write bigrams.bin");
    {
        vector<int> counts;
        counts.insert(counts.end(),m_TypeCounts, m_TypeCounts+3);
        counts.insert(counts.end(),m_TokenCounts, m_TokenCounts+3);
        if (!WriteVector(Path+"/counts.bin", counts))
        throw CExpc ("cannot write counts.bin");
    }
    {
        vector<pair<trigram_integer_t,int> > pairs;
        for (map<trigram_integer_t,int>::const_iterator it=m_Trigrams.begin(); it != m_Trigrams.end(); it++)
            pairs.push_back(pair<trigram_integer_t,int>(it->first, it->second));
        if (!WriteVector(Path+"/trigrams.bin", pairs))
        throw CExpc ("cannot write trigrams.bin");

    }
    if (!WriteVector(Path+"/buckets.bin", m_Buckets))
        throw CExpc ("cannot write buckets.bin");
    if (!WriteVector(Path+"/lambdas.bin", m_BucketsLambdas))
        throw CExpc ("cannot write lambdas.bin");

    
    CTrigramModel M;
    M.m_DictionaryFile = m_DictionaryFile;
    M.ReadBinary();
    if (M.m_bHasSmallTagset  != m_bHasSmallTagset)
        throw CExpc ("m_bHasSmallTagset is badly serialized");
    if (M.m_Bigrams != m_Bigrams)
        throw CExpc ("m_bHasSmallTagset is badly serialized");
    if (M.m_Dictionary  != m_Dictionary)
        throw CExpc ("M.m_Dictionary is badly serialized");
    if (M.m_LexProbs  != m_LexProbs)
        throw CExpc ("m_LexProbs is badly serialized");
    if (M.m_RegisteredTags  != m_RegisteredTags)
            throw CExpc ("m_RegisteredTags is badly serialized");
    if (M.m_TagsCount  != m_TagsCount)
            throw CExpc ("m_TagsCount is badly serialized");
    if (M.m_TagsOrderedByUnigrams  != m_TagsOrderedByUnigrams)
            throw CExpc ("m_TagsOrderedByUnigrams is badly serialized");
    if (M.m_Trigrams  != m_Trigrams)
            throw CExpc ("m_Trigrams is badly serialized");
    if (M.m_TokenCounts[0] != m_TokenCounts[0])
                throw CExpc ("m_TokenCounts is badly serialized");
    if (M.m_TypeCounts[0] != m_TypeCounts[0])
                throw CExpc ("m_TypeCounts is badly serialized");

    return true;
}
 
bool CTrigramModel::ReadBinary()
{
    time_t start;
    time(&start);
    string Path = GetPathByFile(m_DictionaryFile);
    {
        string filename = Path + "/regtags.txt";
        FILE* fp = fopen(filename.c_str(),"r");
        if (!fp) throw CExpc("cannot read %s", filename.c_str());
        char buffer[10000];
        while (fgets (buffer, 10000, fp))
        {
            rtrim(buffer);
            if (strlen(buffer) > 0)
                m_RegisteredTags.push_back(buffer);
        }

        fclose (fp);
        m_TagsCount  = (WORD)m_RegisteredTags.size();
    }
    {
        string filename = Path + "/unigrams.bin";
        ReadVector(filename, m_Unigrams);
        if (m_Unigrams.empty())
            throw CExpc("cannot read %s", filename.c_str());
    }
    {
        string filename = Path + "/ord_tags.bin";
        ReadVector(filename, m_TagsOrderedByUnigrams);
        if (m_TagsOrderedByUnigrams.empty())
            throw CExpc("cannot read %s", filename.c_str());
    }
    {
        string filename = Path + "/bigrams.bin";
        ReadVector(filename, m_Bigrams);
        if (m_Bigrams.empty())
            throw CExpc("cannot read %s", filename.c_str());
    }

    {
        string filename = Path + "/counts.bin";
        vector<int> counts;
        ReadVector(filename, counts);
        if (counts.size() != 6)
            throw CExpc("cannot read %s", filename.c_str());
        for (int i=0; i < 3; i++)
        {
            m_TypeCounts[i] = counts[i]; 
            m_TokenCounts[i] = counts[i+3];
        }
    }

    {
        string filename = Path + "/trigrams.bin";
        vector<pair<trigram_integer_t,int> > pairs;
        ReadVector(filename, pairs);
        if (pairs.empty() )
            throw CExpc("cannot read %s", filename.c_str());
        for (int i=0; i < pairs.size(); i++)
            m_Trigrams[pairs[i].first] = pairs[i].second;
        

    }

    {
        string filename = Path + "/buckets.bin";
        ReadVector(filename, m_Buckets);
        if (m_Buckets.empty())
            throw CExpc("cannot read %s", filename.c_str());
    }
    {
        string filename = Path + "/lambdas.bin";
        ReadVector(filename, m_BucketsLambdas);
        if (m_BucketsLambdas.empty())
            throw CExpc("cannot read %s", filename.c_str());
    }

    if (!read_dictionary_binary())
        return false;

    compute_transition_probs();
    time_t end;
    time (&end);
    fprintf (stderr,"time %i", end-start); 
    return true;
}
