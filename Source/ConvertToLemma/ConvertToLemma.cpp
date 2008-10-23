#include "../common/util_classes.h"
#include "../common/MorphologyHolder.h"




void PrintUsageAndExit()
{
		fprintf (stderr, "Usage: ConvertToLemma <input >output Language ColumnNo (P|\"*\") [-unk][-form] [-unamb]\n");
		fprintf (stderr, "where Language can be Russian, German, English\n");
		fprintf (stderr, "where P is a part of speech and grammems from gramtab.tab \n");
		fprintf (stderr, "if -unk is switched on then it prints only lines with unrecognized words\n");
		fprintf (stderr, " otherwise the program prints lines with all possible possible morph, interpretation\n");
		fprintf (stderr, "if -form is switched on then the program searches input words not only as lemmas but also as word forms\n");
		fprintf (stderr, "if -unamb is switched on then the program prints only lemmas, whose word forms are not ambiguous\n");
		fprintf (stderr, "if -not-norm is switched on then the program prints also words only not lemmas\n");
		fprintf (stderr, "if -print-form is switched on then the program prints initial forms after \"//\" \n");
		fprintf (stderr, "if -print-grammems print grammems\n");
		fprintf (stderr, "if -print-common-grammems print common grammems\n");
		
		
		fprintf (stderr, "\n");
		fprintf (stderr, "Example: ConvertToLemma <test.txt >test.per 1 \"VER|SUB\"\n");
		fprintf (stderr, "ConvertToLemma will conwert all word forms in the first column to lemma (as VER) if it is possible \n");
		fprintf (stderr, "Example: ConvertToLemma <test.txt >test.per 1 *\n");
		fprintf (stderr, "ConvertToLemma will conwert all word forms in the first column to lemma\n");
		exit(1);
};

int main(int argc, char *argv[]) 
{
	bool bPrintOnlyUnknownWords = false;;
	bool bProcessWordForms = false;;
	bool	bPrintOnlyUnambiguous = false;
	bool	bPrintOnlyAmbiguous = false;
	bool	bPrintForm = false;
	bool    bPrintGrammems = false;
	bool    bPrintCommonGrammems = false;
	bool	bPrintOnlyNotLemmas  = false;
	StringVector Argv;

	for (size_t i=1; i<argc; i++)
	{
		std::string s = argv[i];
		EngMakeLower(s);
		if (		(s == "-help") 
				||	(s == "--help") 
				||	(s == "/?") 
			)
			PrintUsageAndExit();
		else
		if (s == "-unk")
		{
			bPrintOnlyUnknownWords = true;
			fprintf (stderr, "print lines with unknown words\n");
		}
		else
		if (s == "-not-norm")
		{
			bPrintOnlyNotLemmas = true;
			fprintf (stderr, "print only not lemmas\n");
		}
		else
		if (s == "-print-form")
		{
			bPrintForm = true;
			fprintf (stderr, "print forms \n");
		}
		else
		if (s == "-print-grammems")
		{
			bPrintGrammems = true;
			fprintf (stderr, "print grammems\n");
		}
		else
		if (s == "-print-common-grammems")
		{
			bPrintCommonGrammems = true;
			fprintf (stderr, "print common grammems\n");
		}
		else
		if (s == "-unamb")
		{
			bPrintOnlyUnambiguous = true;
			fprintf (stderr, "print lines with unambiguous solutions\n");
		}
		else
		if (s == "-amb")
		{
			bPrintOnlyAmbiguous = true;
			fprintf (stderr, "print lines with ambiguous solutions\n");
		}
		else
		if (s == "-form") 
		{
			bProcessWordForms = true;
			fprintf (stderr, "process word forms\n");
		}
		else
			Argv.push_back(argv[i]);
	};

	if (Argv.size() != 3)
		PrintUsageAndExit();
	

	MorphLanguageEnum langua;
	if (!GetLanguageByString(Argv[0], langua)) PrintUsageAndExit();

	CMorphologyHolder Holder;
	if (!Holder.LoadGraphanAndLemmatizer(langua))
	{
		fprintf (stderr,"cannot load morpholgy\n");
		return 1;
	}
	

	int ColumnNo = atoi(Argv[1].c_str());
	fprintf (stderr, "ColumnNo  = %i\n", ColumnNo );
	std::string strPOS = Argv[2];
	size_t PatternPoses = 0;
	QWORD PatternGrammems = 0;
	if (strPOS != "*")
	{
		StringTokenizer tok(strPOS.c_str(), "|");
		while (tok())
		{
			BYTE POS = UnknownPartOfSpeech;
			if ( !Holder.m_pGramTab->ProcessPOSAndGrammemsIfCan(tok.val(), &POS, &PatternGrammems)  )
			{
				fprintf (stderr, "Unrecognized part of speech: %s\n", tok.val());		
				return -1;
			};
			fprintf (stderr, "POS   = %s\n", Holder.m_pGramTab->GetPartOfSpeechStr(POS));
			fprintf (stderr, "Grams = %s\n", Holder.m_pGramTab->GrammemsToStr(PatternGrammems).c_str() );
			PatternPoses |= 1 << POS;
		};
		
	};

	char buffer[10000];
	


	int count = 0;
	StringVector Line;
	StringVector Variants;
	while (fgets (buffer,10000,stdin))
	{
		rtrim(buffer);
		StringTokenizer tok(buffer, "\t;");
		Line.clear();
		int ItemNo = 0;
		
		while (tok()) 
		{
			std::string  t = tok.val();
			Trim(t);
			if (t.empty()) continue;
			Line.push_back( t );
		};

		Variants.clear();

		
		bool bHasSuchLemma = false;
		if (ColumnNo < Line.size()+1)
		{
			string InputWordForm = Line[ColumnNo-1];
			RmlMakeUpper(InputWordForm, langua);
			vector<CFormInfo>  ParadigmCollection; 
			Holder.m_pLemmatizer->CreateParadigmCollection(!bProcessWordForms, InputWordForm, true, false, ParadigmCollection);
			
			for (int j=0; j < ParadigmCollection.size(); j++)
			{
				const CFormInfo& Paradigm = ParadigmCollection[j];
				size_t Poses = 0;
				string Ancodes = Paradigm.GetSrcAncode();
				for (size_t uu=0; uu < Ancodes.size(); uu += 2)
					Poses |= 1 << Holder.m_pGramTab->GetPartOfSpeech(Ancodes.c_str() + uu);

				QWORD Grams = Holder.m_pGramTab->GetAllGrammems(Ancodes.c_str());
				string CommonAncode = Paradigm.GetCommonAncode();
				if (!CommonAncode.empty())
					Grams |= Holder.m_pGramTab->GetAllGrammems(CommonAncode.c_str());

				std::string Norm = Paradigm.GetWordForm(0);
				if (Norm == InputWordForm) 
					bHasSuchLemma = true;

				if (		(PatternPoses == 0 || ( ( PatternPoses & Poses)> 0) )
						&&	(PatternGrammems == 0 || ( ( PatternGrammems & Grams)> 0) )
					)
					if ( std::find(Variants.begin(), Variants.end(), Norm) == Variants.end() )
					{
						if (Paradigm.GetLemmaPrefixLength() != 0)
							Norm.insert(Paradigm.GetLemmaPrefixLength(), "|");

						if (bPrintGrammems)
						{
							QWORD gr;
							Holder.m_pGramTab->GetGrammems(Ancodes.c_str(), gr);
							Norm +=		Format("\t %s %s", 
											Holder.m_pGramTab->GetPartOfSpeechStr(Holder.m_pGramTab->GetPartOfSpeech(Ancodes.c_str())),
											Holder.m_pGramTab->GrammemsToStr(gr).c_str()
											);
						};

						if (bPrintCommonGrammems)
						{
							if (!CommonAncode.empty())
							{
								QWORD cg;
								Holder.m_pGramTab->GetGrammems(CommonAncode.c_str(), cg);
								CommonAncode = Holder.m_pGramTab->GrammemsToStr(cg);
							}
							Norm +=		Format("\t* %s", CommonAncode.c_str()  );
						};
						Variants.push_back(Norm);
					}
			};

		};
		if (bPrintOnlyUnambiguous && (Variants.size() > 1)) continue;
		if (bPrintOnlyAmbiguous && (Variants.size() < 2)) continue;
		if (bPrintOnlyNotLemmas)
		{
			if (!bHasSuchLemma) 
				printf ("%s\n",buffer);
			continue;
		}
		
		if (bPrintOnlyUnknownWords)
		{
			if (Variants.empty())
				printf ("%s\n",buffer);
			continue;
		}
		
		for (int j=0; j < Variants.size(); j++)
		{
			for (int i=0; i<Line.size(); i++)
			{
				if (i+1 ==ColumnNo)
				{
					printf ("%s\t", Variants[j].c_str());
					
				}
				else
				{
					if (isdigit ((unsigned char)Line[i][0]) )
						printf ("%s\t", Line[i].c_str());
					else
						printf ("%s\t", Line[i].c_str());
				};
				
			};

			if (bPrintForm)
			{
				printf (" // %s", Line[ColumnNo-1].c_str() );
			};

			printf ("\n");
		};
	};

  
	return 0;

}