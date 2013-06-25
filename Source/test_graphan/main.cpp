#include <stdio.h>
#include <stdlib.h>
#include <clocale>

#include <comdef.h>
#include <string>

#import "c:/Rml/Bin/Lemmatizer.tlb" 
#import "c:/Rml/Bin/graphan.tlb"

//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LEMMATIZERLib::ILemmatizerPtr initLematizerRu()
{
	printf("Loading russian dictionary of 'Lematizer'...\n");

	LEMMATIZERLib::ILemmatizerPtr piLemmatizerRu; 
	HRESULT hr = piLemmatizerRu.CreateInstance(__uuidof( LEMMATIZERLib::LemmatizerRussian )); 
	if (FAILED(hr)) 
		printf("Russian lemmatizer creation failed.\n"); 
	else
	{
		hr = piLemmatizerRu->LoadDictionariesRegistry(); 
		if (FAILED(hr)) 
			printf("Russian lemmatizer creation failed.\n"); 
	}

	return piLemmatizerRu;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LEMMATIZERLib::ILemmatizerPtr initLematizerEn()
{
	printf("Loading english dictionary of 'Lematizer'...\n");

	LEMMATIZERLib::ILemmatizerPtr piLemmatizerEn; 
	HRESULT hr = piLemmatizerEn.CreateInstance( __uuidof( LEMMATIZERLib::LemmatizerEnglish ) ); 
	if (FAILED(hr)) 
		printf("English lemmatizer creation failed.\n"); 
	else
	{
		hr = piLemmatizerEn->LoadDictionariesRegistry(); 
		if (FAILED(hr)) 
			printf("English lemmatizer creation failed.\n"); 
	}

	return piLemmatizerEn;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
LEMMATIZERLib::ILemmatizerPtr initLematizerDe()
{
	printf("Loading german dictionary of 'Lematizer'...\n");

	LEMMATIZERLib::ILemmatizerPtr piLemmatizerGe; 
	HRESULT hr = piLemmatizerGe.CreateInstance( __uuidof( LEMMATIZERLib::LemmatizerGerman ) ); 
	if (FAILED(hr)) 
		printf("German lemmatizer creation failed.\n"); 
	else
	{
		hr = piLemmatizerGe->LoadDictionariesRegistry(); 
		if (FAILED(hr)) 
			printf( "German lemmatizer creation failed.\n" ); 
	}

	return piLemmatizerGe;
}
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------
GRAPHANLib::IGraphmatFilePtr initGraphan()
{
	printf("Loading dictionary of 'Graphan'...\n");
	
	GRAPHANLib::IGraphmatFilePtr piGraphan;
	HRESULT hr = piGraphan.CreateInstance( __uuidof( GRAPHANLib::GraphmatFile ) );

	if (FAILED(hr))
		printf("Graphan is not registered.\n");
	else
	{
		hr = piGraphan->LoadDicts();
		if (FAILED(hr)) 
			printf("Graphan creation failed.\n"); 
	}

	return piGraphan;
}

char* rtrim (char* s)
{
	size_t len = strlen(s);

	while ( len > 0 && isspace((BYTE)s[len-1]))
		s[--len] = 0;

	return s;

}

bool LoadFileToString(const char* FileName, std::string& Result)
{
    FILE * fp = fopen (FileName, "rb");
	if (fp == NULL) return false;
    char buf [100+1];
    Result = "";
    while (true) {
        size_t act = fread(buf, 1, 100, fp);
        if (act == 0) break;
        buf[act] = 0;
        Result += buf;
    }
	return true;
};

int main(int argc, char** argv)
{
	setlocale(LC_CTYPE, "russian");

	::CoInitialize(NULL); 

	//LEMMATIZERLib::ILemmatizerPtr piLemmatizerRu = initLematizerRu();
	//LEMMATIZERLib::ILemmatizerPtr piLemmatizerEn = initLematizerEn();
	//LEMMATIZERLib::ILemmatizerPtr piLemmatizerDe = initLematizerDe();
    {
	    GRAPHANLib::IGraphmatFilePtr piGraphan = 0;
	    HRESULT hr = piGraphan.CreateInstance( __uuidof( GRAPHANLib::GraphmatFile ) );
		hr = piGraphan->LoadDicts();

        char filename[1024];
        while (fgets(filename, 1024, stdin))
	    {
            rtrim(filename);
            std::string data;
            if (!LoadFileToString(filename, data)) {
                fprintf (stderr, "skip %s\n", filename);
                continue;
            }
            size_t length = data.length();
            fprintf (stderr, "process %s (%i bytes)\n", filename, length);
            try {
                fprintf (stderr, "LoadStringToGraphan\n", filename, length);
                HRESULT hr = piGraphan->LoadStringToGraphan(data.c_str()); 
            }
            catch(...) {
                fprintf (stderr, "unk exception");
                exit(1);
            }

            fprintf (stderr, "FreeTable\n");
		    piGraphan->FreeTable();
            fprintf (stderr, "test\n");
	    }
        fprintf (stderr, "Set null\n");
        piGraphan = 0;
    }
    fprintf (stderr, "exit\n");
	::CoUninitialize(); 

	return EXIT_SUCCESS;
}
