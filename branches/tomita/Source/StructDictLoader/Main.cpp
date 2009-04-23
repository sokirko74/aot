#include "../common/utilit.h"
#include "../StructDictLib/Ross.h"
#include "../StructDictLib/TempArticle.h"

void PrintUsage()
{
		printf ("StructDict Loader (www.aot.ru)\n");
		printf ("This program converts text version of structural dictionary to binary\n");
		printf ("Usage: StructDictLoader <ToTxt|FromTxt> <Inputfile> <OutputCatalog>\n");
		printf ("Example: StructDictLoader ToTxt ross.txt c:/rml/dicts/ross \n");
		exit(-1);	
};

int main(int argc, char **argv)
{
	if (argc != 4)
		PrintUsage();

	if (		!strcmp (argv[1],  "-h")
			||  !strcmp (argv[1],  "-help")
			||  !strcmp (argv[1],  "/h")
			||  !strcmp (argv[1],  "/help")
	)
		PrintUsage();
	string Action = argv[1];
	if	(		(Action != "ToTxt")  
			&&	(Action != "FromTxt")  
		)
		PrintUsage();

	string FileName = argv[2];
	CDictionary Dict;
	if (Action == "FromTxt")
	{
		if (access (FileName.c_str(), 04) != 0)
		{
			fprintf (stderr, "Cannot read %s\n",FileName.c_str());
			return 1;

		};
		if (!Dict.LoadOnlyConstants(argv[3]))
		{
			fprintf (stderr, "Cannot load an empty dictionary from %s\n",argv[3]);
			return 1;
		};
		Dict.m_bShouldSaveComments = true;

		string Messages;
		bool bResult = Dict.ImportFromText(FileName,false, iceSkip,1, Messages);
		fprintf (stderr, "%s", Messages.c_str() );
		if (bResult)
			if (Dict.Save())
				return  0;
		return 1;
	}
	else
	{
		if (!Dict.Load(argv[3]) || !Dict.ReadUnitComments())
		{
			fprintf (stderr, "Cannot load dictionary from %s\n",argv[3]);
			return 1;
		};
		FILE * fp = fopen (FileName.c_str(),"wb");
		if (!fp)
		{
			fprintf (stderr, "Cannot write to %s\n",FileName.c_str());
			return 1;
		};

		CTempArticle A;
		A.m_pRoss = &Dict;

		for (WORD i = 0;  i < Dict.m_Units.size(); i++)
		{ 
			fprintf (fp,"============\r\n");
			fprintf (fp,"%s", Dict.GetUnitTextHeader(i).c_str());
			try 
			{
				A.ReadFromDictionary(i, false, true);
				if (!A.ArticleToText())
				{
					fprintf (fp,"Error! Cannot get the entry No %i\r\n", i);
					return 1;
				};
				fprintf (fp,"%s",A.GetArticleStr().c_str());
			}
			catch (...)
			{
				fprintf (fp,"Error! Cannot get the entry No %i\r\n", i);
				return 1;
			}
			
		};
		fclose(fp);
		return 0;


	};

	
}
