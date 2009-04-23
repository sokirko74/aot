// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "TrigramModel.h"
#include "math.h"
#include "../common/bserialize.h"

#ifdef  USE_TRIGRAM_LEMMATIZER
#include "../RusCorpusXmlComveror/MyStem2Dialing.h"
#endif


const 	int	RareWordThreshold = 1;      /* rare word threshold */


//=================================================

CTrigramModel::CTrigramModel () 
{
	m_Unigrams.clear();
	m_Bigrams.clear();
	m_Trigrams.clear();
	m_TypeCounts[0] = m_TypeCounts[1] = m_TypeCounts[2] = 0;
	m_TokenCounts[0] = m_TokenCounts[1] = m_TokenCounts[2] = 0;
	m_TagsCount = UnknownTag;
	m_bUseSecondLocalMax = false;
    m_bReverseModel = false;
    m_pReverseModel = 0;

#ifdef  USE_TRIGRAM_LEMMATIZER
	m_pLemmatizer = 0;
    m_pLemmatizerPrivate = 0;
	m_pAgramtab = 0;
    m_pAgramtabPrivate = 0;
#endif
	m_MinBucketSize = 0xffffffff;
	m_bDebugViterbi = getenv("DEBUG_VITERBI")!=NULL;
	//m_bDebugViterbi= false;
	//#ifdef  _DEBUG
	//m_bDebugViterbi = true;
	//#endif
	m_bCheckOnlyAmbiguosWords = false;
	m_SecondLocalCoef = 33;
	m_pSmoothedProbsForSmallTagsets = 0;
	m_bRawTexts = false;
	//m_bRusCorporaHtml = false;
	//m_bUseMyStemTags = false;
	m_bQuiet = false;
};



CTrigramModel::~CTrigramModel () 
{
#ifdef  USE_TRIGRAM_LEMMATIZER
	if (m_pLemmatizerPrivate) delete m_pLemmatizerPrivate;
	if (m_pAgramtabPrivate) delete m_pAgramtabPrivate;
	if (m_pSmoothedProbsForSmallTagsets)  delete m_pSmoothedProbsForSmallTagsets;
#endif
	if (m_pReverseModel)
        delete m_pReverseModel;

};


WORD CTrigramModel::find_tag(const string &t) const 
{ 
	assert (!t.empty());
	size_t debug = m_RegisteredTags.size();
	vector<string>::const_iterator it = find(m_RegisteredTags.begin(), m_RegisteredTags.end(), t);
	if (it == m_RegisteredTags.end())
		return UnknownTag;
	else
		return  it - m_RegisteredTags.begin(); 
}

WORD CTrigramModel::register_tag(const string& t)
{
	WORD i = find_tag(t);

	if (i == UnknownTag) 
	{
		m_RegisteredTags.push_back(t);
		i = (WORD)m_RegisteredTags.size() - 1;
	};

	return i;
}



/* ------------------------------------------------------------ */
int CTrigramModel::ngram_index(WORD t1) const
{	
	return t1;
}

int CTrigramModel::ngram_index(WORD t1, WORD t2) const
{	
	return t1*m_TagsCount+t2;
}

trigram_integer_t CTrigramModel::ngram_index(WORD t1, WORD t2, WORD t3) const
{	
	return (t1*m_TagsCount+t2)*m_TagsCount+t3;
}

void CTrigramModel::set_ngram(size_t n, WORD t1, WORD t2, WORD t3, int value) 
{
	assert (m_TagsCount != UnknownTag);

	switch (n)
	{
		case 0: m_Unigrams[ngram_index(t1)] = value; return;
		case 1: m_Bigrams[ngram_index(t1,t2)] = value; return; 
		case 2: m_Trigrams.insert(make_pair(ngram_index(t1,t2,t3), value)); return;
	}
	ErrorMessage(Format("ngram_index %d>2\n", n));
}


size_t CTrigramModel::GetTrigram(WORD t1, WORD t2, WORD t3)  const
{
	// для скорости проверим сначала биграммы, если биграммы пусты, тогда
	// и триграмма пуста
	if ( (m_Bigrams[ngram_index(t1, t2)] == 0) || (m_Bigrams[ngram_index(t2, t3)] == 0)) return 0;

	map<trigram_integer_t,int>::const_iterator it = m_Trigrams.find(ngram_index(t1,t2,t3));

	if (it == m_Trigrams.end())
		return 0;
	else
		return it->second;

}

/* ----------------1------------------------------------------- */




prob_t		CTrigramModel::BuildSmoothedProb(WORD PrevPrevTag, WORD PrevTag, WORD CurrTag) const
{
	prob_t inv_not= 1.0/(prob_t)m_TagsCount;
	int BucketNo = m_Buckets[ngram_index(PrevPrevTag, PrevTag)];
	const CLambdas& L = m_BucketsLambdas[BucketNo];

	int ft3= m_Unigrams[ngram_index(CurrTag)];
	prob_t pt3 = m_TokenCounts[0]>0 ? (prob_t)ft3/(prob_t)m_TokenCounts[0] : inv_not;
	prob_t l1pt3=pt3*L.m_lambda[0];

	int ft2t3 = m_Bigrams[ngram_index(PrevTag, CurrTag)];
	int ft2= m_Unigrams[ngram_index(PrevTag)];
	prob_t pt3_t2 = ft2>0 ? (prob_t)ft2t3/(prob_t)ft2 : inv_not;
	prob_t l2pt3_t2 = pt3_t2 * L.m_lambda[1];
	
	int ft1t2t3 = (int)GetTrigram(PrevPrevTag, PrevTag, CurrTag);
	int ft1t2 = m_Bigrams[ngram_index(PrevPrevTag, PrevTag)];
	prob_t pt3_t1t2 = ft1t2>0 ? (prob_t)ft1t2t3/(prob_t)ft1t2 : inv_not;
	prob_t l3pt3_t1t2=pt3_t1t2* L.m_lambda[2];

	prob_t value = l1pt3 + l2pt3_t2 + l3pt3_t1t2;
	value = log ( value );
	return  value;
};


void CTrigramModel::compute_transition_probs()
{
	
	//m_SmoothedProbsForSmallTagsets.clear();
	if (m_pSmoothedProbsForSmallTagsets)
	{
		delete m_pSmoothedProbsForSmallTagsets;
		m_pSmoothedProbsForSmallTagsets = 0;
	}

	if (m_TagsCount > MaxSmallTagSetSize)
	{
		m_bHasSmallTagset = false;
		m_CachedSmoothedTransProb.clear();
		m_CachedSmoothedSize = 0;
		m_CachedSmoothedTransProb.resize(m_TagsCount);
		return;
	}
	m_bHasSmallTagset = true;
	m_pSmoothedProbsForSmallTagsets = new CTransProbForSmallTagSet;

	fprintf (stderr, "Building all transition(smoothed) probabilities... \n");

	//m_SmoothedProbsForSmallTagsets.resize(m_TagsCount*m_TagsCount*m_TagsCount);
	for (int i=0; i<m_TagsCount; i++)
		for (int j=0; j<m_TagsCount; j++)
			for (int k=0; k<m_TagsCount; k++)
			{
				m_pSmoothedProbsForSmallTagsets->m_Probs[i][j][k] = BuildSmoothedProb(i,j,k);
			}

	

}






CLambdas CTrigramModel::compute_lambdas_for_one_bucket(PMap::const_iterator start_it, PMap::const_iterator end_it)
{
	{
		PMap::const_iterator last_it = end_it;		
		last_it--;
		if (!m_bQuiet)
			fprintf (stderr, "create a bucket with [%i,%i] borders\n", start_it->first, last_it->first);
	}
	int li[3]={0, 0, 0};

	for (PMap::const_iterator it = start_it; it  != end_it; it++)
	{
		const vector< CSingleHistory >& V = it->second;
		for (int i =0; i < V.size(); i++)
		{
			const CSingleHistory& H = V[i];
			int f12 = m_Bigrams[ngram_index(H.i, H.j)]-1;
			if (f12 < 0) continue;
			int f2 = m_Unigrams[ngram_index(H.j)]-1;

			for (WORD k=START_AT_TAG; k<m_TagsCount; k++)
			{
								
				prob_t q[3]={0.0, 0.0, 0.0};
				int f123 = (int)GetTrigram(H.i, H.j, k)-1;
				if (f123<0)  continue; 

				int f23, f3;
				if (m_TokenCounts[0]>1) // если массив состоит хотя бы из двух слов
				{
					f3 = m_Unigrams[ngram_index(k)]-1;
					q[2]=(prob_t)f3/(prob_t)(m_TokenCounts[0]-1);
					if (f2>0)
					{
						f23 = m_Bigrams[ngram_index(H.j, k)]-1;
						q[1]=(prob_t)f23/(prob_t)f2;
						if (f12>0) 
						{ 
							q[0]=(prob_t)f123/(prob_t)f12; 
						}
					}
				}
				int b=0;
				if (q[1]>q[b]) { b=1; }
				if (q[2]>q[b]) { b=2; }
				li[b]+=f123+1;
			}

		};

	}


	int  sum = 0;
	for (int i=0; i<3; i++) 
	{
		sum+=li[i]; 
	}

	CLambdas M;
	for (int i=0; i<3; i++) 
	{ 
		M.m_lambda[i]=(prob_t)li[2-i]/(prob_t)sum; 
	}  
	return M;

}


// вычисляет знаменатель в формуле v(h) = c(h) / |{w : c(h,w) > 0}|
// That is we take the average count over non-zero counts for trigrams.
int CTrigramModel::compute_bucket_denominator(WORD i, WORD j, int& TrigramsCount)
{
	map<size_t,int> TrigramCount2Count;
	TrigramsCount = 0;
	for (WORD k=START_AT_TAG; k<m_TagsCount; k++)
	{
		size_t f123 = GetTrigram(i, j, k);
		if (f123 == 0) continue;
		TrigramsCount++;
		if (TrigramCount2Count.find(f123) == TrigramCount2Count.end())
			TrigramCount2Count[f123] = 1;
		else
			TrigramCount2Count[f123] = TrigramCount2Count[f123]+1;
	}
	if (TrigramCount2Count.empty()) return 1;
	int sum = 0;
	for (map<size_t,int>::const_iterator it = TrigramCount2Count.begin(); it != TrigramCount2Count.end(); it++)
		sum += it->second;
	int size = (int)TrigramCount2Count.size();
	int denom = sum/size;
	if (denom == 0)
	{
		string TagStr1  = m_RegisteredTags[i];
		string TagStr2  = m_RegisteredTags[j];
		int u = 0;
		assert (false);
	};
	return denom;
	
};


//const int MinBucketSize = 100;

bool CTrigramModel::compute_bucketed_lambdas()
{
	
	PMap single_history_values;
	CSingleHistory H;
	for (H.i=START_AT_TAG; H.i<m_TagsCount; H.i++)
	{
		//fprintf(stdout, "\r[%i/%i]      ",H.i,m_TagsCount);

		for (H.j=START_AT_TAG; H.j<m_TagsCount; H.j++)
		{
			int f12 = m_Bigrams[ngram_index(H.i, H.j)];
			int hist_value;
			if (f12 == 0) 
			{
				hist_value = 0;
				H.m_TrigramsCount = 0;
			}
			else
			{
				int denom = compute_bucket_denominator(H.i,H.j, H.m_TrigramsCount);
				if (denom != 1)
				{
					int y = 0;
				};
				hist_value = f12 /denom;
			};
			PMap::iterator it = single_history_values.find(hist_value);
			if (it != single_history_values.end())
				it->second.push_back(H);
			else
				single_history_values[hist_value] = vector< CSingleHistory >(1, H);
		}
	}

	/*
	We make the assumption that whether a bucket is large enough for accurate parameter estimation 
	depends on how many trigrams that fall in that bucket occur in the data used to train the lambda's.
	We bucket in a such way  that a minimum of MinBucketSize  trigrams  fall in each bucket.
								/Chen/
	*/
	m_Buckets.resize(m_TagsCount*m_TagsCount,0);
	m_BucketsLambdas.clear();
	int CurrentBucketNo = 0;
	int CurrentBucketSize = 0;
	PMap::const_iterator start_bucket_it = single_history_values.begin();
	for (PMap::const_iterator it = single_history_values.begin(); it != single_history_values.end(); it++)
	{
		const vector< CSingleHistory >& V = it->second;
		for (int i =0; i < V.size(); i++)
		{
			CurrentBucketSize += V[i].m_TrigramsCount;
			m_Buckets[V[i].i*m_TagsCount+V[i].j] = CurrentBucketNo;
		};

		if (CurrentBucketSize > m_MinBucketSize)
		{
			// process bucket 
			
			PMap::const_iterator end_bucket_it = it;
			end_bucket_it++;
			CLambdas L = compute_lambdas_for_one_bucket(start_bucket_it, end_bucket_it);
			m_BucketsLambdas.push_back(L);

			// increment iterators
			start_bucket_it = it;
			start_bucket_it++;
			CurrentBucketSize = 0;
			CurrentBucketNo++;
		};
	};

	if (start_bucket_it != single_history_values.end())
	{
		CLambdas L = compute_lambdas_for_one_bucket(start_bucket_it, single_history_values.end()); 
		m_BucketsLambdas.push_back(L);
	}
	fprintf (stderr, "Number of buckets = %i\n", m_BucketsLambdas.size());
	return true;	
}







//#pragma optimize( "", off )
void  CTrigramModel::get_tags_from_annots(const vector<CXmlMorphAnnot>& Annots, set<WORD>& tags, const string& WordStr) const
{
    size_t good_annots_count = 0;
    for (size_t i=0; i < Annots.size(); i++)
    {
        string TagStr = Annots[i].BuildRusCorpAnnot();
        if (!TagStr.empty())
        {   
            WORD Tag = find_tag(TagStr);
            if (Tag != UnknownTag)
            {
                tags.insert(Tag);
                good_annots_count++;
            }
            
        }
    }
    if (!Annots.empty())
    {
        if (good_annots_count  == 0)
        {
            fprintf (stderr, "word %s no valid intpretations\n", WordStr.c_str());
            for (size_t i=0; i < Annots.size(); i++)
            {
                string TagStr = Annots[i].BuildRusCorpAnnot();
                fprintf (stderr, "\tgr=%s; tag_str=%s\n", Annots[i].m_GrammemsStr.c_str(), TagStr.c_str());
            }



        }
        assert (good_annots_count > 0);
    }

}


struct LessByWordStr {
	bool  operator() (const CTrigramWord& X, const string& WordStr)  const
    {
        return X.m_WordStr < WordStr;
    }
	bool  operator() (const CTrigramWord& X1, const CTrigramWord& X2)  const
    {
        return X1.m_WordStr < X2.m_WordStr;
    }
	bool  operator() (const string& WordStr, const CTrigramWord& X)  const
    {
        return WordStr < X.m_WordStr;
    }

};


const CTrigramWord* CTrigramModel::lookup_word(const string& s) const
{
    /*map<string,  CTrigramWord>::const_iterator it = m_Dictionary.find(s);
    if (it == m_Dictionary.end()) return 0;
    return &it->second;*/
    vector<CTrigramWord>::const_iterator it = lower_bound(m_Dictionary.begin(), m_Dictionary.end(), s, LessByWordStr());
    if (it == m_Dictionary.end()) return false;
    if (it->m_WordStr != s) return false;
    return &(*it);
}

CDictionarySearch CTrigramModel::find_word(const string& WordStr) const
{
	CDictionarySearch R;
	assert (!WordStr.empty());
	if (WordStr.empty())
	{
		//fprintf (stderr, "Empty word!\n");
        R.m_pFoundWord = 0;
		for (WORD i=0; i < m_TagsCount; i++)
			R.m_PossibleWordTags.insert(i);
		return R;
	}
	
	
    R.m_pFoundWord =  lookup_word(WordStr);

	if (! R.m_pFoundWord ) 
	{
		// если слова нет в словаре, тогда попробуем его поискать в нижнем регистре
		string  lower = WordStr;
		RmlMakeLower(lower, m_Language);
        R.m_pFoundWord =  lookup_word(lower);
	}

	if ( R.m_pFoundWord ) 
	{
		// приписываем все тэги, которые были в корпусе 
        for (size_t i=0; i < R.m_pFoundWord->m_Length; i++)
		{
            int Tag = m_LexProbs[R.m_pFoundWord->m_StartOffset + i].m_Tag;
			R.m_PossibleWordTags.insert(Tag);
		}
	}

	// получаем все возможные тэги из морф. словаря
    map<string, const vector<CXmlMorphAnnot>* >::iterator it = m_CurrentSentenceWords2Annots.find(WordStr);
    if (it != m_CurrentSentenceWords2Annots.end())
        get_tags_from_annots(*it->second,R.m_PossibleWordTags, WordStr);

#ifdef  USE_TRIGRAM_LEMMATIZER
        else
    	    get_tags_from_lemmatizer_but_not_preps(WordStr,R.m_PossibleWordTags);
#endif
		
	if (R.m_PossibleWordTags.empty()) 
		if (		atoi(WordStr.c_str()) > 0
				&&  (m_Language==morphRussian) 
		)
		{
            for (size_t  i=0; i < m_RegisteredTags.size();i++)
                if (m_RegisteredTags[i].length() > 3 && m_RegisteredTags[i].substr(0,4) == "ЧИСЛ")
                    R.m_PossibleWordTags.insert(i);

            if (R.m_PossibleWordTags.empty())
				throw CExpc ("Cannot find ЧИСЛ tag");
		}
		else
		if (		ispunct((BYTE)WordStr[0])
				||	!CheckLanguage(WordStr,m_Language)
			)
		{
			int tag = find_tag("UNK");
			if (tag == UnknownTag)
				throw CExpc ("Cannot find UNK tag");
			R.m_PossibleWordTags.insert(tag);
		}
		else
		{
			
			// приписываем все тэги
			if (!m_bQuiet)
				fprintf (stderr, "No information for word %s\n",WordStr.c_str());
			for (size_t i=0; i < min((size_t)200, m_TagsOrderedByUnigrams.size()); i++)
			{
				WORD tagno = m_TagsOrderedByUnigrams[i];
				string tag = m_RegisteredTags[tagno];
				if (tag.length()> 1 || !ispunct((unsigned char)tag[0]))
					R.m_PossibleWordTags.insert(tagno);
			}
		}

	return R;
}





prob_t CTrigramModel::GetSmoothedLexProb(const CDictionarySearch& DS, WORD PrevTag, WORD CurrTag) const
{
	//see Thede and Harper for explanation of lexical smoothing for second-order HMM 
	prob_t N2 = 0;
	prob_t N3 = 0;
	prob_t MaxTag = 0;
	prob_t AllOccurs = 0;
    if ( DS.m_pFoundWord ) 
	{
        for (size_t i=0; i < DS.m_pFoundWord->m_Length; i++)
		{
            const CLexProb& P = m_LexProbs[DS.m_pFoundWord->m_StartOffset + i];
            // P is one morphological interpretation of current word
			if (MaxTag < m_Unigrams[P.m_Tag])
				MaxTag = m_Unigrams[P.m_Tag];
			if (P.m_Tag == CurrTag)
			{
				N2 += P.m_Count;
				if (P.m_PrevTag == PrevTag)
					N3 = P.m_Count;
			}
			AllOccurs += P.m_Count;
		}
		//assert (N2 > 0);
	}
	prob_t C1 = m_Unigrams[CurrTag];
	prob_t C2 = m_Bigrams[ngram_index(PrevTag, CurrTag)];
	
	prob_t res;
	if ((C2 == 0.0) || (N2 == 0)) //no bigrams (PrevTag, CurrTag) or the input word is unknown
	{
		if (N2 > 0)
		{
			// the input word is known, then return Thede and Harper value
			return log (N2/C1);
		}
		else
		{
			if (MaxTag > 0) 
				// new homonym of this word
				//return log ((prob_t)1/((prob_t)AllOccurs));
				return log ((prob_t)1/((prob_t)(MaxTag)));
			else
				if (C1 > 0)
					// new word
					return log(C1/m_TokenCounts[0]);
				else
					// new tag? is it possible?
					return log(1/(prob_t)m_TokenCounts[0]);
		}
	}
	else
	{
		// best solution
		prob_t PosN3 = log (N3 + 1);
		res = ((PosN3+1)/(PosN3+2))*(N3/C2) +  (1/(PosN3+2)) * (N2/C1);
		// умножаем на длину слова, чем длинне слово - тем меньше оно зависит от контекста
		// надо проверить на полных тэгах
        return log(res* ((prob_t)DS.m_pFoundWord->m_WordStr.length() ));
	}
	
}








// ------------------------------------------------------------ 
void CTrigramModel::dump_transition_probs()
{
	assert (m_TagsCount != UnknownTag );

	for (WORD  i=0; i<m_TagsCount; i++)
	{
		for (WORD j=0; j<m_TagsCount; j++)
		{
			for (WORD k=0; k<m_TagsCount; k++)
			{
				trigram_integer_t index=ngram_index(i, j, k);
				fprintf(stdout, "tp(%s,%s => %s)=%12.11e\n",
					m_RegisteredTags[i].c_str(),
					m_RegisteredTags[j].c_str(),
					m_RegisteredTags[k].c_str(),
					exp(GetSmoothedProb(i,j,k) ));

				fprintf(stdout, "tri(%s,%s,%s)=%d\n",
					m_RegisteredTags[i].c_str(),
					m_RegisteredTags[j].c_str(),
					m_RegisteredTags[k].c_str(),
					GetTrigram(i, j, k) );
			}
			fprintf(stdout, "bi(%s,%s)=%d\n",
					m_RegisteredTags[i].c_str(),
					m_RegisteredTags[j].c_str(),
				m_Bigrams[ ngram_index(i, j) ]);
		}
		fprintf(stdout, "uni(%s)=%d\n",
					m_RegisteredTags[i].c_str(),
			m_Unigrams[ ngram_index(i) ]);
	}
}

// ------------------------------------------------------------ 
// немного исправили эту процедуру для чтения фалов из Яндекса  (лемма пишется в фигурных скобках)


bool CTrigramModel::adjusting_homonyms_coef(string FileName) const
{
	FILE* fp = fopen(FileName.c_str(), "r");  
	if (!fp) 
	{
		fprintf (stderr, "cannot open %s\n", FileName.c_str());
		return false;
	};

	char buffer[10000];
	int pos=0, neg=0, not_amb=0;
	int SentNo = 0;
	while (fgets(buffer, 10000, fp))
	{ 
		SentNo++;
		vector<string> words;
		vector<CWordIntepretation> tags;
		vector<WORD> refs;

		StringTokenizer tok(buffer, " \t\r\n");
		for (size_t i=0; tok(); i++)
		{
			string t = tok.val();

			if (i%2==0) 
			{ 
				words.push_back(t); 
			}
			else
			{
				WORD ti=find_tag(t);
				if (ti == UnknownTag) 
				{
					fprintf (stderr, "unknown tag \"%s\" LineNo=%i\n", t.c_str(), SentNo);
					return false;
				}
				refs.push_back(ti);
			}
		}
		if (words.empty())  continue;

		if ( (SentNo%100)== 0)
			printf ("\r%i   ", SentNo);

		if (!viterbi(words, tags))
			return false;


		for (size_t i=0; i<words.size(); i++)
		{
			
			WORD ref = refs[i];
			if (tags[i].m_TagId2 == UnknownTag)
				not_amb++;

			if (		(tags[i].m_TagId1==ref) 
					||	(tags[i].m_TagId2==ref)
				)
				pos++; 
			else
			{
				string gs = m_RegisteredTags[tags[i].m_TagId1];
				if (tags[i].m_TagId2 != UnknownTag)
					gs += "["+m_RegisteredTags[tags[i].m_TagId2]+"]";
				string rs = m_RegisteredTags[ref];
				
				
				for (int j=(int)i-5; j<(int)min(i+5, words.size()); j++)
				{
					if (j<0) { continue; }
					

					if (j != i)
						fprintf(stderr, "%s ", words[j].c_str());
					else
						fprintf(stderr, "*%s* [guess %s ref %s] ", words[i].c_str(), gs.c_str(), rs.c_str() );	
				}
				fprintf(stderr, "\n");	
				

				neg++;	      
			}
		}
		//fprintf (stderr, "dump %d\n",pos+neg);
	}
	printf ("\n");

	fprintf (stderr, "%d (%d+%d) words tagged, accuracy %7.3f%%, recall %7.3f%%\n",
		pos+neg, pos, neg, 100.0*(prob_t)pos/(prob_t)(pos+neg), 100.0*(prob_t)not_amb/(prob_t)(pos+neg));

	fclose (fp);
	return true;
}

void CTrigramModel::print_tag_set() const
{
	for (WORD i=0;i<m_RegisteredTags.size(); i++)
	{
		printf ("%s %i\n",  
			m_RegisteredTags[i].c_str(), 
			m_Unigrams[ngram_index(i)]);
	};

};



void CTrigramModel::InitModelFromConfigAndBuildTagset(string FileName, const CLemmatizer* Lemmatizer, const CAgramtab* GramTab, bool bLoadReverseModel) 
{
	FILE * fp = fopen (FileName.c_str(), "r");
	if (!fp)
		throw CExpc ("cannot read file %s\n", FileName.c_str());
	string TagsetFile;
    string ReverseModelConfig;
    char buffer[1000]; 
	while (fgets(buffer, 1000, fp))
	{
        if (strchr (buffer, '#'))
            *strchr (buffer, '#') = 0;

		StringTokenizer tok(buffer, "\r\n\t ");
		if (!tok())
			 continue;
		string Field = tok.val();
        string Value;
		if (tok())
		    Value = tok.val();
		if (Field == "NgramFile")
            m_NgramFile = BuildRMLPath (Value.c_str());
		else
		if (Field == "DictionaryFile")
            m_DictionaryFile = BuildRMLPath (Value.c_str());
		else
		if (Field == "TagsetFile")
			TagsetFile = BuildRMLPath (Value.c_str());
		else
		if (Field == "Language")
        {
			if (!GetLanguageByString(Value, m_Language))
                throw CExpc ("unknown language:%s\n",  Value.c_str());
        }
        else
		if (Field == "--second-local-coef")
		{
			m_SecondLocalCoef = atoi(Value.c_str());
			if (!m_SecondLocalCoef)
                throw CExpc ("wrong second local coef: %s\n",  Value.c_str());
			fprintf(stderr, "second local coef: %i\n",  m_SecondLocalCoef);
            m_bUseSecondLocalMax = true;	
		}
		else
		if (Field == "--min-bucket-size")
		{
			m_MinBucketSize = atoi(Value.c_str());
			if (!m_MinBucketSize)
				throw CExpc ("wrong min bucket size: %s\n",  Value.c_str());
			fprintf(stderr, "min bucket size: %i\n",  m_MinBucketSize);
		}
		else
		if (Field == "reverse-model-config")
		{
            
                ReverseModelConfig = Value;
       	}
		else
		if (Field == "--raw-texts")
		{
			m_bRawTexts = true;
		}
		else
		if (Field == "--supress-morph-errors")
		{
			m_bQuiet = true;
		}
        else
		if (Field == "--reverse-model")
		{
			m_bReverseModel = true;	
			fprintf (stderr, "reverse model!\n");
		}
		else
		if (Field == "--check-only-amb-words")
		{
			m_bCheckOnlyAmbiguosWords = true;	
			fprintf (stderr, "evaluate precision and recall only for ambiguous words\n");
		}

	};
	fclose(fp);
	if (		m_NgramFile.empty()
			||	m_DictionaryFile.empty()
		) 
        throw CExpc ("cannot find  NgramFile or DictionaryFile in %s\n", FileName.c_str());

#ifdef  USE_TRIGRAM_LEMMATIZER
    if (GramTab==0)
    {
	    if (!InitDicts())
            throw CExpc ("cannot initialize morphology\n", FileName.c_str());
    }
    else
    {
        m_pLemmatizer = Lemmatizer;
        m_pAgramtab = GramTab;
        // m_Graphan is disabled
    };
   	if (!TagsetFile.empty())
	{
		fprintf (stderr, "loading tagset from %s\n", TagsetFile.c_str());
	    if (!m_TagSet.ReadTagSet(TagsetFile, m_pAgramtab))
		    throw CExpc ("cannot load tagset");
	}
	else
	{
		fprintf (stderr, "building default tagset\n");
		m_TagSet.BuildDefaultTags(m_pAgramtab);
	}
	fprintf (stderr, "tag set file contains %i tags \n", m_TagSet.m_Tags.size());
#endif

    if (bLoadReverseModel && !ReverseModelConfig.empty())
    {
       m_pReverseModel = new CTrigramModel();
       fprintf(stderr, "load reverse model from : %s\n",  ReverseModelConfig.c_str());
       #ifdef  USE_TRIGRAM_LEMMATIZER
        m_pReverseModel->InitModelFromConfigAndBuildTagset(ReverseModelConfig, m_pLemmatizer, m_pAgramtab, false);
       #else
           m_pReverseModel->InitModelFromConfigAndBuildTagset(ReverseModelConfig, 0, 0, false);
       #endif
       m_pReverseModel->ReadBinary();
    }
};




bool CTrigramModel::DisambiguateRusCorpXml(vector<CXmlToken>& Words)  const
{
	vector<string> tokens;
	vector<CWordIntepretation> tags;
    m_CurrentSentenceWords2Annots.clear();    
    for (size_t WordNo=0; WordNo < Words.size(); WordNo++)	
    {
        const CXmlToken& W = Words[WordNo];
        tokens.push_back( W.m_WordStr );
        m_CurrentSentenceWords2Annots[W.m_WordStr] = &W.m_Annots;
    }

    if (!viterbi(tokens, tags))
		    	return false;
    for (size_t WordNo=0; WordNo < Words.size(); WordNo++)	
    {
        CXmlToken& W = Words[WordNo];
        WORD besttag = tags[WordNo].m_TagId1;
        size_t i=0;
        vector<CXmlMorphAnnot> NewAnnots;
        for (;  i <W.m_Annots.size(); i++)
        {
            string TagStr = W.m_Annots[i].BuildRusCorpAnnot();
            if (!TagStr.empty())
            {
                WORD tag = find_tag(TagStr);
                if (tag == besttag)
                    NewAnnots.push_back(W.m_Annots[i]);
            }
        }
        NewAnnots.swap(W.m_Annots);
    }
	return true;
}



