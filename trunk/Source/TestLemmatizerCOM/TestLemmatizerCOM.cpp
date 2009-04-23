//================================================================
// =====   COM Test Example for Dialing Lemmatizer (www.aot.ru) ==
// =====   Author: Alexey Sokirko, sokirko@yandex.ru, 2003  =======
//================================================================

#include <stdio.h>
#include <comdef.h>
#import "../../bin/Lemmatizer.tlb"

int main(int argc, char *argv[]) 
{
	::CoInitialize(NULL);

	//  creating and loading Russian dictionary
	LEMMATIZERLib::ILemmatizerPtr	piLemmatizer;
	HRESULT hr = piLemmatizer.CreateInstance(__uuidof(LEMMATIZERLib::LemmatizerRussian));
	if (FAILED(hr))
	{
		fprintf(stderr, "Fatal Error: Lemmatizer creation failed");
		return 1;
	}
	// loading dictionary into the memory
	hr = piLemmatizer->LoadDictionariesRegistry();
	if (FAILED(hr))
	{
		printf("Fatal Error: Lemmatizer loading failed");
		return false;
	}


	// lemmatizing a word
    {
	    LEMMATIZERLib::IParadigmCollectionPtr piParadigmCollection = piLemmatizer->CreateParadigmCollectionFromForm("мама", FALSE, FALSE);

	    //  print possible lemmas
	    for (int j=0; j < piParadigmCollection->Count; j++)
	    {
		    _bstr_t Lemma = 		_bstr_t(piParadigmCollection->Item[j]->Norm);
		    printf ("Lemma %i = \"%s\"\n", j+1, (const char*)Lemma);
	    };
    }




	// destroy the dictionary
	piLemmatizer = 0;

	::CoUninitialize();	

}
