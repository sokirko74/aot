using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET;
using LemmatizerNET.Files;
using System.IO;
using System.IO.Compression;
using System.Globalization;

namespace LemmatizerNetTest {
	class Program {
		static void Main(string[] args) {
			var rmlPath = @"c:\RML\";
			Console.WriteLine("For test LemmatizerNET you need Lemmatizer dictionaries (RML)");
			Console.Write("\tRML directory ("+rmlPath+"): ");
			var newRmlPath=Console.ReadLine();
			if (!string.IsNullOrEmpty(newRmlPath)){
				rmlPath=newRmlPath;
			}
			
			Console.Write("Select language 'R'-Russian, 'G'-German, 'E'-English (default - R): ");
			var langStr=Console.ReadLine().ToUpper(CultureInfo.InvariantCulture);
			MorphLanguage lang;
			switch(langStr){
				case "":
				case "R":
					lang=MorphLanguage.Russian;
					break;
				case "G":
					lang=MorphLanguage.German;
					break;
				case "E":
					lang=MorphLanguage.English;
					break;
				default:
					Console.WriteLine("Wrong selection. Using default language Russian");
					lang=MorphLanguage.Russian;
					break;
			}
			ILemmatizer lem = LemmatizerFactory.Create(lang);
			try{
				var manager = FileManager.GetFileManager(rmlPath);
				lem.LoadDictionariesRegistry(manager);
			} catch(IOException e){
				Console.WriteLine("Unable to load dictionaries due to the following:\r\n\t");
				Console.WriteLine(e.Message);
				return;
			}
			while(true){
				Console.Write("Input word (q - exit): ");
				var word=Console.ReadLine();
				if(word.Equals("q", StringComparison.InvariantCultureIgnoreCase)){
					return;
				}
				var paradigmList = lem.CreateParadigmCollectionFromForm(word,false,false);
				if (paradigmList.Count == 0) {
					Console.WriteLine("Paradigms not found");
					continue;
				}
				for(var i=0;i<paradigmList.Count ;i++){
					var paradigm = paradigmList[i];
					Console.Write("Paradigm: ");
					Console.WriteLine(paradigm.Norm);
					Console.Write("\tFounded: ");
					Console.WriteLine(paradigm.Founded);
					Console.Write("\tParadigmID: ");
					Console.WriteLine(paradigm.ParadigmID);
					for(var j=0;j<paradigm.Count;j++){
						Console.Write("\t\t");
						Console.Write(paradigm.GetForm(j));
						Console.Write("\t");
						Console.WriteLine(paradigm.GetAncode(j));
					}
				}
			}
		}
	}
}
