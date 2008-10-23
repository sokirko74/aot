// MorphGen.cpp : Defines the entry point for the console application.
//
#pragma warning(disable:4786)
#pragma warning(disable:4503)

#include "../common/utilit.h"
#include "../LemmatizerLib/StdMorph.h"
#include "../LemmatizerLib/Predict.h"
#include "../MorphWizardLib/wizard.h"
#include "../LemmatizerLib/MorphDictBuilder.h"


void PrintUsage()
{
		printf ("Dialing Morphological Generator (from text format (*.mrd) to binary(*.bin))\n");
		printf ("Usage: MorphGen <mwz-file> OutputCatalog <PostfixLength> <MinFreq> [--allow-russian-jo]\n");
		printf ("where <mwz-file> is a morphology project file\n");
		printf ("<PostfixLength> is the length of the predicting postfix (can be 1,2,3,4,5). \n");
		printf ("     By these postfixes will be unkonwn words predicted\n");
		printf ("     The size of the prediction base depends crucialy upon this value\n");
		printf ("<MinFreq> is the minimal frequence of the predicting postfix in the dictionary\n");
		printf ("if --allow-russian-jo is swithched, then the program does not convert Russian 'jo' to 'e'\n");
		
		printf ("Example: MorphGen rus.mwz c:/RUS_BIN/ 3 2\n");
		exit(1);
}; 

string GetPrefixPartOfSpeech (MorphLanguageEnum Langua)
{
	switch (Langua) {
		case morphRussian: return  "ÏÐÈÑÒ";
		case morphEnglish: return  "PREFIX";
		case morphGerman: return  "PREFIX";
		default :return "unk";
	}
};


extern bool GenerateMorphDict(std::string filename, std::string out_path, int PostfixLength, int MinFreq);




int main(int argc, char* argv[])
{
	bool bAllowRussianJo = false;
	if(argc == 6) 
		if (string(argv[5]) == "--allow-russian-jo")
		{
			bAllowRussianJo = true;
			argc = 5;
		}
		else
			PrintUsage();

	int PostfixLength = atoi(argv[3]);
	if (	(PostfixLength == 0) ||  (PostfixLength > 5) )
	{
		printf ("PostfixLength should be between 1 and 5\n");
		return 1;
	};

	int MinFreq  = atoi(argv[4]);
	if (MinFreq <= 0)
	{
		printf ("MinFreq should be more than 0\n");
		return 1;
	};

	std::string  Error;
	if(!IsRmlRegistered(Error))
	{
		printf ("%s\n",Error.c_str());
		return 1;
			
	};


	try{

		//  adding "/" to the end of  OutputPath
		std::string OutputPath = argv[2];
		if (!OutputPath.empty())
		{
			char c = OutputPath[OutputPath.length() - 1];
			if (		(c != '/')
					&&	(c != '\\')
				)
			OutputPath += '/';
		};

		MorphoWizard Wizard;
		std::string WizardFilename = argv[1];
		if (!Wizard.load_wizard(WizardFilename.c_str(), "guest",false))
		{	
			fprintf (stderr,"Cannot load mrd-file : %s\n", WizardFilename.c_str());
			return 1;
		};

		if (!bAllowRussianJo)
		{
			fprintf (stderr,"prepare_for_RML\n");
			if (!Wizard.prepare_for_RML())
				return 1;
		};

		{
			CMorphDictBuilder R(Wizard.m_Language);


			if (!R.GenerateLemmas(Wizard))
			{
				fprintf (stderr,"Cannot generate lemmas \n");
				return 1;
			};
		
			if (!R.GenerateUnitedFlexModels(Wizard))
			{
				fprintf (stderr,"Cannot generate flex models \n");
				return 1;
			};


			if (!R.CreateAutomat(Wizard )) 
			{
				fprintf (stderr,"Cannot create the main automat \n");
				return 1;
			};

			fprintf (stderr,"Saving...\n");
			if (!R.Save(OutputPath+MORPH_MAIN_FILES)) 
			{
				fprintf (stderr,"Cannot save the main automat to %s\n", std::string(OutputPath+MORPH_MAIN_FILES).c_str());
				return 1;
			};
			fprintf (stderr,"Successful written indices of the main automat to %s\n", std::string(OutputPath+MORPH_MAIN_FILES).c_str());

			fprintf (stderr,"test\n");

		
			if (!R.GenPredictIdx(Wizard, PostfixLength, MinFreq, OutputPath))
			{
				fprintf (stderr,"Cannot create prediction base \n");
				return 1;
			};
		}

		
		{ // generating options file
			string OptFileName = OutputPath+OPTIONS_FILE;
			fprintf(stderr, "writing options file %s\n", OptFileName.c_str());
			FILE * opt_fp = fopen(OptFileName.c_str(),"w");
			if (!opt_fp)
			{
				fprintf (stderr,"Cannot write to file %s \n",OptFileName.c_str());
				return 1;
			};
			if (bAllowRussianJo)
				fprintf (opt_fp,"AllowRussianJo\n");
			fclose (opt_fp);
			fprintf (stderr,"Options file was created\n");
		}

	}
	catch(CExpc c)
	{
		printf("Can not Generate: %s\n", c.m_strCause.c_str());
		return 1;
	}
	catch(...)
	{
		printf("Can not Generate\n");
		return 1;
	}
	fprintf(stderr, "exit\n");

	return 0;
}


