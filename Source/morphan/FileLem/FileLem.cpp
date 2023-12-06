
//================================
// ==========   Dialing Lemmatizer (www.aot.ru)
//================================
#include "morph_dict/common/utilit.h"
#include "../LemmatizerLib/LemTextCreator.h"


void PrintUsage()
{
		printf ("Dialing File Lemmatizer(www.aot.ru)\n");
		printf ("Usage: FileLem (RUSSIAN|ENGLISH|GERMAN) <input>\n");
		printf ("Example: FileLem Russian input_list.txt\n");
		printf ("<input> is a file with list of files (one file per line)\n");
		printf ("Possible file encodings: Russian(windows-1251), German(windows-1252)\n");
		exit(-1);	
};

int main(int argc, char **argv)
{
	// ===============  LOADING DICTS ================
	if (argc != 3)
		PrintUsage();

	if (		!strcmp (argv[1],  "-h")
			||  !strcmp (argv[1],  "-help")
			||  !strcmp (argv[1],  "/h")
			||  !strcmp (argv[1],  "/help")
	)
		PrintUsage();

	MorphLanguageEnum Language;
	if (!GetLanguageByString(argv[1], Language))
	{
		PrintUsage();
	};
	fprintf (stderr, "Loading dictionaries \n");
	CLemTextCreator Holder;
	Holder.LoadGraphanAndLemmatizer(Language);

	
	const char* FileName = argv[2];
	FILE * fp = fopen (FileName, "r");
	if (!fp)
	{
		std::cout << "  Cannot open " << FileName << "\n";
		return 1;
	};
	char buffer[1024];
	while (fgets(buffer, 1024, fp))
	{
		std::string f = buffer;
		Trim(f);
		int CountOfWords;
		std::cout << "Reading " << f <<  "\n";
		if (!Holder.BuildLemText(f,true, CountOfWords)) 
		{
			std::cout << "  Cannot process " << f << "\n";
			continue;
		};
		std::cout << "Found " << Holder.m_LemText.m_LemWords.size() <<" items\n";
		std::string LemFile = MakeFName(f, "lem");
		std::cout << "Save to " << LemFile  << "\n";
		Holder.m_LemText.SaveToFile(LemFile);
	};
	return 0;
}
