/*
 This example loads the input file into graphematics and prints 
 all found English words.
*/

#include <stdio.h>
#include "string.h"
#include <string>
#import "../../bin/graphan.tlb"




int main(int argc, char* argv[])
{	
	if (argc != 2)
	{
		printf ("TestComGraphan, Test for Dialing Graphematical Analysis(COM)(www.aot.ru)\n");
		printf ("Usage: TestComGraphan <file>\n");
		printf ("where file is an text or html file\n");
		return 1;
	};
	// init COM
	CoInitialize(NULL);

	GRAPHANLib::IGraphmatFilePtr piGraphan;
	
	// creating COM-object
	HRESULT hr = piGraphan.CreateInstance(__uuidof(GRAPHANLib::GraphmatFile));
	if (FAILED (hr))
	{
		printf ("Graphan is not registered");
		CoUninitialize();
		return 1;
	};

	// loading dictionaries 
	hr = piGraphan->LoadDicts();

	piGraphan->Language = 1;

	// write the graphematical table to .gra file 
	piGraphan->GraOutputFile = "out.gra";

	// write the graphematical macro syntax structure .xml file 
	piGraphan->XmlMacSynOutputFile = "out.xml";

	//  loading file
	printf ("Loading file %s\n", argv[1]);
	piGraphan->LoadFileToGraphan(argv[1]);
	

	// get the number of rows in the graphematical table
	size_t Count = piGraphan->GetLineCount();
	printf ("Found Units %i\n", Count);

	// print all words which are written in English ABC
    for (size_t i =0; i< Count; i++)
		if (piGraphan->HasDescr(i,GRAPHANLib::OLLE) == TRUE)
	   {
		   std::string s = (const char*) piGraphan->GetWord (i);  // get the token
		   printf ("%s\n", s.c_str()); 

	   };
	piGraphan = 0;
	CoUninitialize();
	return 0;
}


/*
#include "stdio.h"
#include "string.h"
#include "string"
#import "../../bin/graphan.tlb"//  импортируембиблиотеку
 
// String Example
int main (int argc, char* argv[])
{
  try
  {
      // инициализируем библиотеку COM
      CoInitialize(NULL);
      // инициализируем переменную класса IGraphmatFilePtr
      GRAPHANLib::IGraphmatFilePtr piGraphan;
      HRESULT hr = piGraphan.CreateInstance(__uuidof(GRAPHANLib::GraphmatFile));
      if (FAILED (hr))
      {
            printf ("Graphan is not registered");
            CoUninitialize();
            return 1;
      };
 
      piGraphan.CreateInstance(__uuidof(GRAPHANLib::GraphmatFile));
 
      piGraphan->LoadDicts();
 
      // загружаем строчку в графематику 
      piGraphan->LoadStringToGraphan("мама мыла раму");
 
      _bstr_t t = piGraphan->GetWord(3);
 
      assert (t == _bstr_t("раму"));
  }
  catch(...)
  {
            return -1;
  }
  CoUninitialize();
  return 1;
}

*/