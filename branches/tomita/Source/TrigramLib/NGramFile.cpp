// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Lemmatizer (www.aot.ru)
// ==========  Copyright by Alexey Sokirko

#include "TrigramModel.h"


class IsGreaterByUnigrams {
public:
	const vector<int>&			m_Unigrams;
	IsGreaterByUnigrams(const vector<int>& Unigrams): m_Unigrams(Unigrams) {};
	bool operator () (const WORD& i1, const WORD& i2) const 
	{
		return m_Unigrams[i1] > m_Unigrams[i2];
	}
};

// 
const size_t MaxTagsCount  = 3000;

bool CTrigramModel::register_tags_from_ngram_file()
{
	FILE* fp = fopen(m_NgramFile.c_str(), "r");
	if (!fp) 
	{
		fprintf (stderr, "cannot open %s\n", m_NgramFile.c_str());
		return false;
	};
	
	
	/* tag 0 is special: begin of sentence & end of sentence */
	m_RegisteredTags.clear();
	register_tag("<eos>");
	char buffer[1024];
	while (fgets(buffer, 1024, fp))
	{
		if (buffer[0]=='\t') { continue; }
		string q = strtok(buffer,"\t ");;
		register_tag(q);
        if (m_RegisteredTags.size() > MaxTagsCount)
        {
            fclose (fp);
            throw CExpc ("too many tags; cannot be more than %i\n", MaxTagsCount);
        }
	};

	m_TagsCount = (WORD)m_RegisteredTags.size();
	fprintf (stderr, "found %d tags in \"%s\"\n", m_TagsCount-1, m_NgramFile.c_str());
    return true;
}

bool CTrigramModel::read_ngram_file()
{
	FILE* fp = fopen(m_NgramFile.c_str(), "r");
	if (!fp) 
	{
		fprintf (stderr, "cannot open %s\n", m_NgramFile.c_str());
		return false;
	};
	

	{
		for (int i=0; i<3; i++)
		{
			m_TypeCounts[i]=0;
			m_TokenCounts[i]=0; 
		}
		m_Unigrams.resize(m_TagsCount, 0);
		m_Bigrams.resize(m_TagsCount*m_TagsCount,0);
	}


	int lno = 0;
	WORD t[3]={0, 0, 0};
    char buffer[1024];
    while (fgets(buffer, 1024, fp))
    {
		lno++;
		
		char *s = buffer;
		int i=0;
		for (; *s=='\t'; i++, s++) { /* nadо */ }

		if (i>2) 
		{
			fprintf(stderr, "parse error (too many tabs) (%s:%d)\n", m_NgramFile.c_str(), lno); 
		}

		s=strtok(s, " \t");
		if (!s) 
		{ 
			fprintf(stderr, "can't find tag (%s:%d)\n", m_NgramFile.c_str(), lno); 
			return false;
		}
		t[i]=find_tag(s);
		if (t[i] == UnknownTag) 
		{ 
			fprintf(stderr, "unknown tag \"%s\" (%s:%d)\n", s, m_NgramFile.c_str(), lno); 
			return false;
		}

		s=strtok(NULL, " \t");
		if (!s) 
		{ 
			fprintf(stderr, "can't find count (%s:%d)\n", m_NgramFile.c_str(), lno); 
			return false;
		}
		int cnt;
		if (1!=sscanf(s, "%d", &cnt)) 
		{
			fprintf(stderr, "can't read count (%s:%d)\n", m_NgramFile.c_str(), lno); 
			return false;
		}
		set_ngram(i, t[0], t[1], t[2], cnt);
		m_TypeCounts[i]++;
		m_TokenCounts[i]+=cnt;
    }

		
	fprintf(stderr, "read %d/%d uni-, %d/%d bi-, and %d/%d trigram count (type/token)\n",
		m_TypeCounts[0], m_TokenCounts[0], m_TypeCounts[1], m_TokenCounts[1], m_TypeCounts[2], m_TokenCounts[2]);

	m_TagsOrderedByUnigrams.resize(m_TagsCount);
	for (WORD i=0;i < m_TagsCount; i++)
		m_TagsOrderedByUnigrams[i] = i;
	sort (m_TagsOrderedByUnigrams.begin(), m_TagsOrderedByUnigrams.end(), IsGreaterByUnigrams(m_Unigrams));
	
	
	
	return true;
}
/* ------------------------------------------------------------ */
void CTrigramModel::compute_counts_for_boundary()
{
	assert (m_TagsCount != UnknownTag);
	/* compute transition probs for artificial boundary tags */
	int uni=0, bi=0, tri=0, ows=0, nos=0;

	/* we don't start at zero because of the boundary tags */
	
	for (WORD i=1; i<m_TagsCount; i++)
	{
		int bx, xb, bx_=0;
		//fprintf(stdout, "\r[%i/%i]      ",i,m_TagsCount);

		bx = xb = m_Unigrams[ ngram_index(i) ];
		WORD j=1;
		for (; j<m_TagsCount; j++)
		{ 
			int bxy, xyb;

			bx-= m_Bigrams[ ngram_index(j, i) ];
			xb-= m_Bigrams[ ngram_index(i, j) ];

			bxy = xyb = m_Bigrams[ ngram_index(i, j) ];
			if (bxy > 0) // если bxy = 0, тогда следующий цикл не будет осмысленным
			for (WORD k=1; k<m_TagsCount; k++)
			{
				bxy-= (int)GetTrigram( k, i, j );
				xyb-= (int)GetTrigram( i, j, k );
			}
			bx_+=bxy;

			set_ngram(2, 0, i, j, bxy);
			set_ngram(2, i, j, 0, xyb);
			tri+=bxy+xyb;
		}
		/* Boundary unigrams, two at the beginning, one at the end */
		uni+=bx+bx+xb;
		/* For each start of a sentence, there must be tags t-2, t-1. */
		m_Bigrams[ ngram_index(0, 0) ] += bx;
		bi+=bx;

		/* t-1, w1 */
		m_Bigrams[ ngram_index(0, i) ] = bx;
		bi+=bx;

		/* wn, t+1 */
		m_Bigrams[ ngram_index(i, 0) ]=xb;
		bi+=xb;

		/* (t-2, t-1, w1) */
		set_ngram(2, 0, 0, i, bx);
		tri+=bx;

		/*
		FIXME: really true?
		This compensates the lack of a continuation of the sentence.
		For bigrams, we assume that tag t+1 has a successor t+2.
		*/
		m_Bigrams[ ngram_index(0, 0) ]+=bx;
		bi+=bx;


		/*
		FIXME: really true?
		Wrong again. There are no triples <0,0,0> of course.
		However, we added bigram counts for <0,0> for tag t+1
		(see above) and here we add corresponding (artificial)
		trigrams.
		*/
		set_ngram(2, 0, 0, 0, (int)GetTrigram( 0, 0, 0 ) + bx);
		tri+=bx;


		/*
		FIXME:
		Strictly speaking, the following is wrong. There's only one
		boundary tag at the end. But we need trigrams <x, 0, _>.
		See above.
		Below, the corresponding (real) bigram is added.
		*/
		set_ngram(2, i, 0, 0, xb);
		tri+=xb;

		/* This is for one-word sentences: t-1, w1, t+1 */
		j=bx-bx_;
		set_ngram(2, 0, i, 0, j);
		tri+=j;
		ows+=j;
	}

	/* TODO: check what to use */
	m_Unigrams[ ngram_index(0) ]=uni; /* 0? uni? uni/3? */

	nos=uni/3;
	m_TokenCounts[0] += 3*nos;
	m_TokenCounts[1] = m_TokenCounts[0];
	m_TokenCounts[2] = m_TokenCounts[0];
	fprintf(stderr, "model generated from %d sentences (thereof %d one-word)\n", uni/3, ows);
	fprintf(stderr, "found %d uni-, %d bi-, and %d trigram counts for the boundary tag\n", 
		uni, bi, tri);
}


