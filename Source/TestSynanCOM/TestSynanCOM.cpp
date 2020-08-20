/*
 This is an example of how to use COM-interfaces of Dialing Syntax.  
 The program processes a sentence, and then prints all found syntax groups 

*/
#include "../common/COM/COMSyntaxHolder.h"
#include "../common/utilit.h"
#include <fstream>

int main(int argc, char* argv[])
{
	::CoInitialize(0);
	{
		// loading Russian syntax
		CCOMSyntaxHolder	SyntaxHolder;
		if (!SyntaxHolder.LoadSyntax(morphRussian)) {
			std::cerr << "cannot initialize syntax\n";
			return 1;
		}

		std::string inputFile = argv[1];
		assert(access(inputFile.c_str(), 0) != -1);
		std::cout << "Loading file " << inputFile << "\n";

		// processing one Russian sentence (from Graphematics to Syntax)
		if (!SyntaxHolder.GetSentencesFromSynAn(inputFile.c_str(), TRUE, TRUE, FALSE)) {
			std::cerr << "cannot process the input file\n";
			return -1;
		}



		// printing all syntax groups

		// going through all sentences
		std::ofstream outp(inputFile + ".synan");

		for(int i = 0 ; i < SyntaxHolder.m_piSentCollection->SentencesCount ; i++ )
		{
			auto piSent = SyntaxHolder.m_piSentCollection->GetSentence(i);

			// going through all clauses in this sentence
			for(int j = 0 ; j < piSent->ClausesCount ; j++ )
			{
				auto piClause = piSent->Clause[j];
				//  getting the first morphological variant
				if (piClause->VariantsCount == 0)
				{
					std::cerr << " Error! A clause with no morph. variant is found\n"; 
					continue;
				};

				// going through all syntax groups
				for(int k = 0 ; k < piClause->ClauseVariant[0]->GroupsCount ; k++ )
				{
					auto piGroup = piClause->ClauseVariant[0]->Group[k];
					auto s = piGroup->FirstWord;
					auto l = piGroup->LastWord;
					outp << piGroup->TypeStr << "[" << s << ", " << l << "]\n";
				}

			}
			
		}

	}
	::CoUninitialize();
	return 0;
}
