
#include "TrigramModel.h"
#include "math.h"



// ------------------------------------------------------------ 
//Extend viterbi() so that it can also work in multiple-tags
//mode.
//- Add parameter ``prob_t probs[]''.
//- if probs==NULL we are in best-sequence mode.
//- if probs!=NULL we are in multi-tag mode
//Probs is then a pre-allocated C-array of size not*WordsCount.
//- The probability that word w_i has tag t_j is stored in
//probs[i*not+j].
//- Change declaration of a to ``prob_t a[WordsCount][not][not]''
//- Adapt updating of next_ai and curr_ai.
//- When finished, the traverse b[][][] and a[][][] and enter
//infos in probs.

/* ----------------1------------------------------------------- */


prob_t		CTrigramModel::GetSmoothedProb(uint16_t PrevPrevTag, uint16_t PrevTag, uint16_t CurrTag) const
{
	if (m_bHasSmallTagset)
		return m_pSmoothedProbsForSmallTagsets->m_Probs[PrevPrevTag][PrevTag][CurrTag];


	
	const BMap& M = m_CachedSmoothedTransProb[PrevPrevTag];
	size_t index = ngram_index(PrevTag, CurrTag);
	BMap::const_iterator it = M.find(index);
	if (it != M.end())
		return it->second;

	prob_t  value = BuildSmoothedProb(PrevPrevTag, PrevTag, CurrTag);
	if (m_CachedSmoothedSize < m_MaxCacheSize)
	{
		m_CachedSmoothedTransProb[PrevPrevTag][index] = value;
		m_CachedSmoothedSize++;
		//if ((m_CachedSmoothedSize%100) == 0)
		//	fprintf (stderr, "cache size = %i\n",m_CachedSmoothedSize);
	}
	return  value;
};

inline prob_t  mod(const prob_t& x)
{
	if (x < 0) 
		return -x;
	else
		return x;
};




void CTrigramModel::BuildReverseLexProb(const std::vector<std::string>& words, std::vector<std::map<uint16_t,prob_t> >& RevLexProbs) const 
{
    RevLexProbs.resize(words.size(), std::map<uint16_t,prob_t>() );
    if (!m_pReverseModel) return;
    int WordsCount = words.size();    

    std::set<uint16_t> PrevTags;
    PrevTags.insert(0);
    for (int WordNo=(int)WordsCount-1; WordNo>=0; WordNo--)
    {
        CDictionarySearch DS = m_pReverseModel->find_word(words[WordNo]);
        std::map<uint16_t,prob_t>& distr =  RevLexProbs[WordNo];
        for (std::set<uint16_t>::const_iterator curr_it=DS.m_PossibleWordTags.begin(); curr_it!=DS.m_PossibleWordTags.end(); curr_it++) // a tag for current word
            for  (std::set<uint16_t>::const_iterator prev_it = PrevTags.begin(); prev_it!=PrevTags.end(); prev_it++)
            {
                uint16_t PrevTag = *prev_it;
                uint16_t CurrTag = *curr_it;
                prob_t LexProb =  m_pReverseModel->GetSmoothedLexProb(DS, PrevTag, CurrTag);
                std::map<uint16_t,prob_t>::iterator it = distr.find(CurrTag);
                if (it != distr.end())
                    it->second = std::max (it->second, LexProb);
                else
                    distr[CurrTag] = LexProb;
            }

       PrevTags.swap(DS.m_PossibleWordTags);
    }  
    for (size_t WordNo=0; WordNo < WordsCount; WordNo++)
    {
        std::map<uint16_t,prob_t>& distr =  RevLexProbs[WordNo];
        std::map<uint16_t,prob_t> new_distr;
        for (std::map<uint16_t,prob_t>::const_iterator curr_it=distr.begin(); curr_it!=distr.end(); curr_it++) 
        {
            uint16_t t = curr_it->first;
            new_distr[ find_tag(m_pReverseModel->m_RegisteredTags[t]) ] = curr_it->second;
        }
        distr.swap(new_distr);
    }
}
void CTrigramModel::ViterbiForward(const std::vector<std::string>& words, std::vector<CViterbiInfo>& Triplet, const std::vector<CViterbiInfo>& RevTriplet) const 
{
	if (m_bDebugViterbi)
	{
		fprintf(stderr, "\n===========  ViterbiForward =============== \n\n");
		fprintf(stderr, "\n Number of word:%i\n", words.size());
	}
	size_t WordsCount = (int)words.size();
	Triplet[0].SetProb(0,0, 0.0);
	std::set<uint16_t> OuterBoundTags;
	OuterBoundTags.insert(0);
    std::vector<std::map<uint16_t,prob_t> > RevLexProbs;
    BuildReverseLexProb(words, RevLexProbs);
    

	for (size_t WordNo=0; WordNo<WordsCount; WordNo++)
	{
		CViterbiInfo& I = Triplet[WordNo];
		// можно сделать кеш по 10000 самых частотных слов
		CDictionarySearch DS = find_word(words[WordNo]);
			
		I.m_LexicalProbs = DS.m_PossibleWordTags;
		bool bUnknownWord = (I.m_LexicalProbs.size() == m_TagsCount);

		if (m_bDebugViterbi)
		{
			fprintf(stderr, "\n===========  Word \"%s\"   =============== \n\n", words[WordNo].c_str());
		}
					

		for (std::set<uint16_t>::const_iterator curr_it=I.m_LexicalProbs.begin(); curr_it!=I.m_LexicalProbs.end(); curr_it++) // a tag for current word
		{
			uint16_t CurrTag = *curr_it;
			const std::set<uint16_t>& PrevTags = (WordNo<1) ? OuterBoundTags : Triplet[WordNo-1].m_LexicalProbs;
			for (std::set<uint16_t>::const_iterator prev_it=PrevTags.begin(); prev_it != PrevTags.end(); prev_it++) // the tag of the previous word
			{
				uint16_t PrevTag = *prev_it;
				prob_t LexProb =  GetSmoothedLexProb(DS, PrevTag, CurrTag);
                prob_t RevLexProb = RevLexProbs[WordNo][CurrTag]/2;
                if (m_pReverseModel)
                    LexProb /= 2;

				const std::set<uint16_t>& PrevPrevTags = (WordNo<2) ? OuterBoundTags : Triplet[WordNo-2].m_LexicalProbs;
				for (std::set<uint16_t>::const_iterator prev_prev_it=PrevPrevTags.begin(); prev_prev_it != PrevPrevTags.end(); prev_prev_it++) // the tag of the previous of the previous word
				{
					uint16_t PrevPrevTag = *prev_prev_it;
					
					prob_t new_value = I.GetProb(PrevPrevTag,PrevTag) + GetSmoothedProb(PrevPrevTag, PrevTag, CurrTag) + LexProb + RevLexProb;
					if (new_value == 0)
					{
						fprintf (stderr, "Error! A zero transition probability has occurred!\n");
						exit(1);
					};

					if (m_bDebugViterbi)
					{
						#define TN(x) m_RegisteredTags[x].c_str()
						fprintf(stderr, "Considering < %s %s %s >\n",TN(PrevPrevTag), TN(PrevTag), TN(CurrTag));
						fprintf(stderr,"\ta(%s %s)==%5f\n", TN(PrevPrevTag), TN(PrevTag), I.GetProb(PrevPrevTag,PrevTag));
						fprintf(stderr, "\tLexProb(%s,%s|%s)==%5f\n", TN(PrevTag), TN(CurrTag), words[WordNo].c_str(), LexProb);
						fprintf(stderr,"\tSmoothedProb(%s %s %s)==%5f\n",TN(PrevPrevTag), TN(PrevTag), TN(CurrTag), GetSmoothedProb(PrevPrevTag, PrevTag, CurrTag) );
						fprintf(stderr,"\tFreq(%s %s %s)==%u\n",TN(PrevPrevTag), TN(PrevTag), TN(CurrTag), GetTrigram(PrevPrevTag, PrevTag, CurrTag) );
						fprintf(stderr,"\t---> %5f\n", new_value);
					}

					
					if ( new_value > Triplet[WordNo+1].GetProb(PrevTag,CurrTag) )
					{
						I.SetTagRef(PrevTag,CurrTag, PrevPrevTag);
						Triplet[WordNo+1].SetProb(PrevTag,CurrTag, new_value);
					}

					// сохраняем локальные максимумы
					if (m_bUseSecondLocalMax)
                    {
						if (WordNo >= 2)
						{
                            Triplet[WordNo-2].UpdateMaximum(new_value,PrevPrevTag);
						};
                        Triplet[WordNo].UpdateMaximum(new_value,CurrTag);
                    }
                        

				}
			}
		}
	}
}


void CTrigramModel::ViterbiBackward(const std::vector<std::string>& words, const std::vector<CViterbiInfo>& Triplet, std::vector<CWordIntepretation>& tags) const 
{
	if (m_bDebugViterbi)
	{
		fprintf(stderr, "\n===========  ViterbiBackward =============== \n\n");
		
	}
	size_t WordsCount = words.size();
	// find highest prob in last column  and save it to  b_a,  CurrTag, PrevTag
	
	uint16_t PrevTag=1, CurrTag=1;
	
    {
        prob_t b_a = -MAXPROB;
		std::map<std::pair<uint16_t,uint16_t>, prob_t> FinalProbs = Triplet.back().GetAllProbs();
	    for (std::map<std::pair<uint16_t,uint16_t>, prob_t>::const_iterator it = FinalProbs.begin();  it != FinalProbs.end(); it++)
	    {
		    uint16_t i = (uint16_t)it->first.first;
		    uint16_t j = (uint16_t)it->first.second;
		    prob_t new_value = it->second + GetSmoothedProb( i, j, 0) ;
		    if (m_bDebugViterbi)
		    {
			    fprintf(stderr, "Considering <%s-%s> as the best final state\n", TN(i), TN(j));
			    //fprintf(stderr, "\ta(%s-%s)==%5f\n", TN(i).c_str(), TN(j).c_str(), Triplet.back().GetProbs(i,j));
			    fprintf(stderr, "\ttp(%s-%s --> %s-%s)==%5f\n",
				    TN(i), TN(j), TN(j), "NULL", GetSmoothedProb( i, j, 0) );
			    fprintf(stderr, "\t---> %5f\n", new_value );
		    }
		    if (new_value > b_a) 
		    {
			    b_a = new_value; 
                PrevTag = i;
			    CurrTag = j; 
		    }

	    }
    }
	
	tags.resize(WordsCount);
	
	for (int WordNo=(int)WordsCount-1; WordNo>=0; WordNo--)
	{
		tags[WordNo].m_TagId1 = CurrTag;

	
		uint16_t PrevPrevTag = Triplet[WordNo].GetTagRef(PrevTag,CurrTag);

        if (Triplet[WordNo].IsAmbig() )
		{
            std::pair<prob_t,uint16_t> First = Triplet[WordNo].GetFirstMaximum();
            std::pair<prob_t,uint16_t> Second = Triplet[WordNo].GetSecondMaximum();
            if (First.second != CurrTag)
		    {
			    tags[WordNo].m_TagId2 = First.second;
		    }
		    else
                if	( (First.first - Second.first) < mod(First.first)/(prob_t)m_SecondLocalCoef)
                {
                    tags[WordNo].m_TagId2 = Second.second;

				    //fprintf (stderr, "difference between alternative tag (%s) and the best(%s) is %2.2f%% for %s\n", m_Tags[I0.m_CurrentTag].c_str(),m_Tags[I1.m_CurrentTag].c_str(),100*mod(I0.m_ProbValue - I1.m_ProbValue)/mod(I0.m_ProbValue),words[WordNo].c_str());

			    };
		};

		CurrTag = PrevTag;
		PrevTag = PrevPrevTag;
	}
};


bool CTrigramModel::viterbi(const std::vector<std::string>& words, std::vector<CWordIntepretation>& tags) const 
{
    assert (m_TagsCount != UnknownTag );
    if (m_bReverseModel)
    {
        
        std::vector<std::string> words_r = words;
        reverse(words_r.begin(), words_r.end());
        std::vector<CViterbiInfo> Triplet(words.size()+1);
        ViterbiForward(words_r, Triplet, std::vector<CViterbiInfo>());
	    ViterbiBackward(words_r, Triplet, tags);
        reverse(tags.begin(), tags.end());

    }
    else
    {
        std::vector<CViterbiInfo> Triplet(words.size()+1);
        /*if (m_pReverseModel)
        {
            std::vector<CViterbiInfo> RevTriplets(words.size()+1);
            std::vector<std::string> words_r = words;
            reverse(words_r.begin(), words_r.end());
            m_pReverseModel->ViterbiForward(words_r, RevTriplets, std::vector<CViterbiInfo>());
           	ViterbiForward(words, Triplet, RevTriplets);
        }
        else*/
            ViterbiForward(words, Triplet, std::vector<CViterbiInfo>());

	    ViterbiBackward(words, Triplet, tags);
    }
	return true;
}

#ifdef  USE_TRIGRAM_LEMMATIZER

bool CTrigramModel::testing(std::string FileName) const
{
	clock_t start = clock();
	FILE* fp = fopen(FileName.c_str(), "r");  
	if (!fp) 
	{
		fprintf (stderr, "cannot open %s\n", FileName.c_str());
		return false;
    	};

	char buffer[10000];
	int pos=0, neg=0, interps_count=0;
	int pos_lemma=0, neg_lemma=0;
	int SentNo = 0;
	while (fgets(buffer, 10000, fp))
	{ 
		SentNo++;
		std::vector<std::string> words;
		std::vector<CWordIntepretation> tags;
		std::vector<size_t> refs;

		StringTokenizer tok(buffer, " \t\r\n");
		for (size_t i=0; tok(); i++)
		{
			std::string t = tok.val();

			if (i%2==0) 
			{ 
				words.push_back(t); 
			}
			else
			{
				uint16_t ti=find_tag(t);
				refs.push_back(ti);
			}
		}
		if (words.empty())  continue;

		if ( (SentNo%10)== 0)
			printf ("\r%i   ", SentNo);

        //reverse (words.begin(),  words.end());
		if (!viterbi(words, tags))
			return false;
        //reverse (words.begin(),  words.end());
        //reverse (tags.begin(),  tags.end());
        

		for (size_t i=0; i<words.size(); i++)
		{
			
			uint16_t ref = (uint16_t)refs[i];

			// не берем в расчет знаки препинания и слова, записанные латиницей					
			if (!CheckLanguage(words[i], m_Language))
				continue;

			if (m_bCheckOnlyAmbiguosWords)
			{
				// если  указана опция --check-only-amb-words,
				// тогда берем в расчет только слова, у которых больше одной леммы
				std::vector<CFormInfo> Paradigms;
				std::string WordStr = words[i];
				m_pLemmatizer->CreateParadigmCollection(false, WordStr, is_upper_alpha((BYTE)WordStr[0], m_Language), true, Paradigms);
				std::set<std::string> UniqueLemmas;
				for (int h=0; h < Paradigms.size(); h++)
					UniqueLemmas.insert(Paradigms[h].GetWordForm(0));
				if (UniqueLemmas.size() <= 1)
					continue;
			}

			interps_count++;
			if (tags[i].m_TagId2 != UnknownTag)
				interps_count++;

			if (		(tags[i].m_TagId1==ref) 
					||	(tags[i].m_TagId2==ref)
				)
			{
					pos++; 
					pos_lemma++;
			}
			else
			{

				std::string rs = (ref == UnknownTag) ? "unk_tag" : m_RegisteredTags[ref];
				bool bLemmaDiff =		!(		CheckLemma(words[i], m_RegisteredTags[tags[i].m_TagId1], rs)
											||	(		(tags[i].m_TagId2 != UnknownTag)
													&&	CheckLemma(words[i], m_RegisteredTags[tags[i].m_TagId2], rs)
												)
										);
				if (!bLemmaDiff)
					pos_lemma ++;
				else
					neg_lemma ++;
				
				std::string debug_tag = m_RegisteredTags[tags[i].m_TagId1];
				if (tags[i].m_TagId2 != UnknownTag)
					debug_tag += "["+m_RegisteredTags[tags[i].m_TagId2]+"]";
				for (int j=(int)i-5; j<(int)std::min(i+5, words.size()); j++)
				{
					if (j<0) { continue; }
					if (j != i)
						fprintf(stderr, "%s ", words[j].c_str());
					else
					{
						fprintf(stderr, "*%s* [guess %s ref %s] ", words[i].c_str(), debug_tag.c_str(), rs.c_str() );	
					};
				}
				if (bLemmaDiff)
						fprintf(stderr, " <lemma_diff> ");
				fprintf(stderr, "\n");	

				neg++;	      
			}
		}
		//fprintf (stderr, "dump %d\n",pos+neg);
	}
	printf ("\n");

	fprintf (stderr, "%d (%d+%d) words tagged, accuracy %7.2f%%, avg.amb %7.2f\n",
		pos+neg, pos, neg, 
		100.0*(prob_t)pos/(prob_t)(pos+neg), 
		(prob_t)interps_count/(prob_t)(pos+neg));

	fprintf (stderr, "Lexical accuracy %7.2f%% (positive=%i, negative=%i)\n", 100.0*(prob_t)pos_lemma/(prob_t)(pos_lemma+neg_lemma), pos_lemma, neg_lemma);

	fclose (fp);
	clock_t end = clock();
	printf ("Ticks %i\n", end-start);
	return true;
}

#endif

    
