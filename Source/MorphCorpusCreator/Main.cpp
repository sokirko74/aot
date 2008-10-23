// DialingProcessor.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "MorphCorpusCreator.h"

int main(int argc, char* argv[])
{
	::CoInitialize(NULL);
	{
		if( argc < 4 )
		{
			printf("Version 1.7 \n");
			printf("Usage: MorphCorpusCreaator <lang> <source_file_name> <result_file_name> [-gr] [-cl] [-sah ][-sem:<filename>]. \n");
			printf(" <lang> can be German or Russian\n ");
			printf(" -cl - print clauses info \n");
			printf(" -sah - save all homonyms \n");
			printf(" -sem:<path to file with semantic charachteristics>\n");
			return 0;
		}

		MorphLanguageEnum Language;
		if (!GetLanguageByString(argv[1], Language))
		{
			printf(" bad language: \"%s\"\n", argv[1]);
			exit(1);
		};

		CCOMSyntaxHolder Holder;
		if (Holder.LoadSyntax(Language) != TRUE)
		{
			printf(" cannot load syntax \n");
			exit(1);
		};
		Holder.m_piMAPost->bCanChangeInputText = FALSE;


		CMorphCorpusCreator pr;

		for(int i = 3 ; i < argc ; i++ )
		{

			if( !strcmp(argv[i], "-cl") )
				pr.m_b_show_clause = true;

			if( !strcmp(argv[i], "-sah") )
				Holder.m_piSentCollection->KillHomonymsMode = DontKillHomonyms;

		}

		string in_file(argv[2]);		
		Holder.m_piSentCollection->EnableProgressBar = TRUE;

		if (!Holder.GetSentencesFromSynAn(in_file, TRUE, FALSE, FALSE))
		{
			printf(" cannot process file \n");
			exit(1);
		};

		int iSentCount = Holder.m_piSentCollection->SentencesCount;
		cerr << iSentCount << " sentences processed by SynAn" << endl;		


		string result = pr.get_xml_result(Holder);

		ofstream file;
		file.clear();
		file.open(argv[3], ofstream::binary|ofstream::out);
		file.write(result.c_str(), result.size() + 1);
		file.close();

		return 0;
	}
	::CoUninitialize();
}

