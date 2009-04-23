/*
 This is an example of how to use COM-interfaces of Dialing Syntax.  
 The program processes a sentence, and then prints all found syntax groups 

*/
#include "../common/COMSyntaxHolder.h"
#include "../common/utilit.h"

int main(int argc, char* argv[])
{
	::CoInitialize(0);
	


	{
		// loading Russian syntax
		CCOMSyntaxHolder	SyntaxHolder;
		if (!SyntaxHolder.LoadSyntax(morphRussian))
			return -1;

		// processing one Russian sentence (from Graphematics to Syntax)
		if (!SyntaxHolder.GetSentencesFromSynAn("мама мыла раму", FALSE, FALSE, FALSE))
			return -1;

		// printing all syntax groups

		// going through all sentences
		for(int i = 0 ; i < SyntaxHolder.m_piSentCollection->SentencesCount ; i++ )
		{
			SYNANLib::ISentencePtr piSent = SyntaxHolder.m_piSentCollection->GetSentence(i);

			// going through all clauses in this sentence
			for(int j = 0 ; j < piSent->ClausesCount ; j++ )
			{
				SYNANLib::IClausePtr piClause = piSent->Clause[j];
				//  getting the first morphological variant
				if (piClause->VariantsCount == 0)
				{
					printf (" Error! A clause with no morph. variant is found\n"); 
					continue;
				};

				// going through all syntax groups
				for(int k = 0 ; k < piClause->ClauseVariant[0]->GroupsCount ; k++ )
				{
					
					SYNANLib::IGroupPtr piGroup = piClause->ClauseVariant[0]->Group[k];
					SYNANLib::IWordPtr piW1 = piSent->Word[piGroup->FirstWord];
					SYNANLib::IWordPtr piW2 = piSent->Word[piGroup->LastWord];
					printf("(%s %s %s )\n", 
							(const char*)piGroup->TypeStr, 
							(const char*)piW1->WordStr, 
							(const char*)piW2->WordStr
							);				
				}

			}
			
		}

	}
	::CoUninitialize();
	return 0;
}
