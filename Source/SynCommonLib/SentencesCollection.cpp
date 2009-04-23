// ==========  This file is under  LGPL, the GNU Lesser General Public Licence
// ==========  Dialing Syntax Analysis (www.aot.ru)
// ==========  Copyright by Dmitry Pankratov, Igor Nozhov, Alexey Sokirko


#include "stdafx.h"
#include "SentencesCollection.h"

// language specific functions
extern CSyntaxOpt*	NewOptionsRussian ();
extern CSyntaxOpt*	NewOptionsGerman ();
extern CSyntaxOpt*	NewOptionsEnglish ();

 CSyntaxOpt*  NewOptions (MorphLanguageEnum langua)
{
	if (langua == morphGerman)
		return NewOptionsGerman();

	if (langua == morphEnglish)
		return NewOptionsEnglish();
	
	return NewOptionsRussian();
};

CSentencesCollection::CSentencesCollection() 
{	
	m_bDeleteEqualPrimitiveClauseVariants = false;
	m_bEnableProgressBar = false;
	m_bLogProcessedSentence = false;
	m_pSyntaxOptions = 0;

}
void CSentencesCollection::ClearOptions()
{
	if (m_pSyntaxOptions)
	{
		if(m_pSyntaxOptions->m_bSilentMode )
			m_pSyntaxOptions->CloseLogFile();

		m_pSyntaxOptions->DestroyOptions();
		delete m_pSyntaxOptions;
		m_pSyntaxOptions = 0;
	};
};


void CSentencesCollection::ClearSentences() 
{	
	for (int i = 0; i < m_vectorSents.size();  i++)
		delete m_vectorSents[i];

	ClearVector(m_vectorSents); 
};


CSentencesCollection::~CSentencesCollection()
{	
	ClearSentences();
	ClearOptions();
}


bool CSentencesCollection::CreateOptions(MorphLanguageEnum langua)
{
	try
	{
		ClearOptions();
		m_pSyntaxOptions = NewOptions(langua);
		assert (m_pSyntaxOptions);
	}
	catch(...)
	{
		return false;
	}
	return true;
}



bool CSentencesCollection::InitializeProcesser()
{
	try
	{
		if (!m_pSyntaxOptions->InitializeOptions()) return false;
	}
	catch(...)
	{
		return false;
	}
	return true;
}


bool AreEqualByWords (const CMorphVariant& _X1, const CMorphVariant& _X2) 
{
	if (_X1.GetUnitsCount() != _X2.GetUnitsCount())
		return false;

	for(int i = 0 ; i < _X1.GetUnitsCount() ; i++ )
	  if  (   (_X1.GetUnitType(i) != EClause)
		   || (_X2.GetUnitType(i) != EClause)
		  )
		if(_X1.GetHomNum(i) != _X2.GetHomNum(i))
				return false;

	return true;	
};

bool CSentencesCollection::ProcessData(const CPlmLineCollection* piPlmLine)
{
	try
	{
		bool bRes = ReadAndProcessSentences(piPlmLine);

		/*
		  см. комментарий к  m_bDeleteEqualPrimitiveClauseVariants
		*/
		if (m_bDeleteEqualPrimitiveClauseVariants)
		{
            for (long i =0; i < m_vectorSents.size(); i++)
			{
				CSentence&  Clauses = *(m_vectorSents[i]);

				for (long ClauseNo=0; ClauseNo < Clauses.GetClausesCount(); ClauseNo++)
				{
					CClause& C = Clauses.GetClause(ClauseNo);
					for (SVI j = C.m_SynVariants.begin(); j != C.m_SynVariants.end(); j++)
					{
						SVI k = j;
						k++;


						while(k != C.m_SynVariants.end())
							if ( AreEqualByWords(*j, *k) )
								{
									k = C.m_SynVariants.erase(k);
								}
							else
								k++;
					};
				};
			};
		};


		if( !bRes )
			return false;

	
	}
	catch(...)
	{
		return false;
	}

	return true;
}



bool CSentencesCollection::ReadAndProcessSentences(const CPlmLineCollection* piPlmLine)
{
	const size_t LinesCount =  piPlmLine->m_Items.size();
	
	if( !m_pSyntaxOptions->IsValid())
			return false;
	

	if (m_bEnableProgressBar)
			printf ("Starting Syntax\n");

	time_t t1;
	time  (&t1);
	
	for (size_t LineNo = 0; LineNo < LinesCount; ) 
	{
		if (m_bEnableProgressBar)
				printf ("%i of %i       \r", LineNo, LinesCount);

		CSentence* S = m_pSyntaxOptions->NewSentence();
		if (!S)
		{
			m_pSyntaxOptions->OutputErrorString("Cannot allocate space for the new sentence!");
			return false;
		};
		try
		{

			S->m_pSyntaxOptions = m_pSyntaxOptions;

			bool bResult = S->ReadNextFromPlmLines(piPlmLine,LineNo);

			if (m_bLogProcessedSentence)
			{
				FILE * fp = fopen ("last_read_sentence.log", "w");
				fprintf (fp, "%s\n",S->GetSentenceBeginStr().c_str());
				fclose(fp);
			};

			if( !bResult )
			{
				//  a parse error occurs 
				delete S;
				return false;		
			};

		}
		catch (...)
		{

			m_pSyntaxOptions->OutputErrorString("Cannot read a sentence from Mapost");
			delete S;
			return false;
		};

		
		if( S->m_Words.empty() )
		{
			// no not-empty sentence can be found, we are at the end of the text
			delete S;
			break;
		}


		try
		{
			bool bRes = S->BuildClauses();

			if( !bRes )
			{
				delete S;
				return false;
			};

			S->CalculatePrimitiveClausesCount();

			
			if (m_bEnableProgressBar)
			{
				if (S->m_bPanicMode)
					printf ("    found a \"panic\" sentence\n");
			};

			m_vectorSents.push_back(S);

		}
		catch (...)
		{
			if (m_bEnableProgressBar)
			{
				printf ("\n");
				if (!S->m_Words.empty())
					printf ("exception at offset %i\n", S->m_Words[0].m_GraphematicalUnitOffset);
			};
			 
			delete S;
			m_pSyntaxOptions->OutputErrorString("An exception in Synan occurred!");
			return false;
		};
		
		
	}

	if (m_bEnableProgressBar)
	{
		time_t t2;
		time  (&t2);
		printf ("Finish             \n");
		int seconds = t2-t1;
		printf ("Time : %i\n", seconds);
		printf ("Count of tokens : %i\n", LinesCount);
		if (seconds > 0)
			printf ("The speed is %i tokens pro second\n", LinesCount/seconds);
	};
	return true;
}






void CSentencesCollection::put_SilentMode(bool newVal)
{
	try
	{
		if( !m_pSyntaxOptions->m_bSilentMode )
			m_pSyntaxOptions->OpenLogFile();

		if( !m_pSyntaxOptions->m_bSilentMode )
			m_pSyntaxOptions->CloseLogFile();

		m_pSyntaxOptions->m_bSilentMode = newVal;
	}
	catch(...)
	{
		ErrorMessage("SynAn", "Failed to open logfile.");	
	}
}





void CSentencesCollection::SetLemmatizer(const CLemmatizer* R)
{
	m_pSyntaxOptions->SetLemmatizer(R, false);
}

void CSentencesCollection::SetOborDic(const CDictionary* D)
{
	m_pSyntaxOptions->SetOborDic(D, false);
}

/*
void CSentencesCollection::SetThesaurus(UINT ThesId, const CThesaurus* Thes)
{
	m_pSyntaxOptions->GetThesPointerByThesId(ThesId).SetPointer(Thes, false);

};
*/
void CSentencesCollection::SetEnableAllThesauri(bool Value)
{
	m_pSyntaxOptions->m_bEnableCompThesaurus = Value;
	m_pSyntaxOptions->m_bEnableLocThesaurus = Value;
	m_pSyntaxOptions->m_bEnableFinThesaurus = Value;
	m_pSyntaxOptions->m_bEnableOmniThesaurus = Value;
};
