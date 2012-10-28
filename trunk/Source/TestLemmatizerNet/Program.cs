using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET;
using LemmatizerNET.Files;
using System.IO;
using System.Text.RegularExpressions;
using System.IO.Compression;
using System.Globalization;

namespace LemmatizerNetTest
{
    class Program
    {
        static void Main(string[] args)
        {
            var rmlPath = System.Environment.GetEnvironmentVariable("RML");
            Console.WriteLine("For test LemmatizerNET you need Lemmatizer dictionaries (RML)");
            Console.Write("\tRML directory (" + rmlPath + "): ");

            if (string.IsNullOrEmpty(rmlPath))
            {
                var newRmlPath = Console.ReadLine();
                if (!string.IsNullOrEmpty(newRmlPath))
                {
                    rmlPath = newRmlPath;
                }
            }

            //Console.Write("Select language 'R'-Russian, 'G'-German, 'E'-English (default - R): ");
            var langStr = "R"; // Console.ReadLine().ToUpper(CultureInfo.InvariantCulture);
            MorphLanguage lang;
            switch (langStr)
            {
                case "":
                case "R":
                    lang = MorphLanguage.Russian;
                    break;
                case "G":
                    lang = MorphLanguage.German;
                    break;
                case "E":
                    lang = MorphLanguage.English;
                    break;
                default:
                    Console.WriteLine("Wrong selection. Using default language Russian");
                    lang = MorphLanguage.Russian;
                    break;
            }
            ILemmatizer lem = LemmatizerFactory.Create(lang);
            string rgt = "";
            try
            {
                StreamReader r = new StreamReader(rmlPath + @"\Dicts\Morph\" + langStr.ToLower() + "gramtab.tab", Encoding.GetEncoding(1251));
                rgt = r.ReadToEnd(); r.Close();
            }
            catch (Exception e)
            {
            }
            try
            {
                var manager = FileManager.GetFileManager(rmlPath);
                lem.LoadDictionariesRegistry(manager);
            }
            catch (IOException e)
            {
                Console.WriteLine("Unable to load dictionaries due to the following:\r\n\t");
                Console.WriteLine(e.Message);
                return;
            }
            while (true)
            {
                Console.Write("\nInput word (q - exit): ");
                var word = Console.ReadLine().Replace("\"", "").Replace("'", "").Trim();
                if (word.Equals("q", StringComparison.InvariantCultureIgnoreCase))
                {
                    return;
                }
                var paradigmList = lem.CreateParadigmCollectionFromForm(word, false, false);
                if (paradigmList.Count == 0)
                {
                    Console.WriteLine("Paradigms not found");
                    continue;
                }
                for (var i = 0; i < paradigmList.Count; i++)
                {
                    var paradigm = paradigmList[i];

                    Console.Write("Paradigm: ");
                    rmlPath = paradigm.SrcAncode;
                    Console.WriteLine(paradigm.Norm);
                    int k = paradigm.GetAccent(0);
                    k = paradigm.SrcAccentedVowel;
                    Console.Write("\tFounded: ");
                    Console.WriteLine(paradigm.Founded);
                    Console.Write("\tParadigmID: ");
                    Console.WriteLine(paradigm.ParadigmID);
                    Console.Write("\tAccentModelNo: ");
                    Console.WriteLine(paradigm.AccentModelNo);
                    Console.WriteLine("=====");
                    Console.WriteLine("$type_grm = " + (paradigm.TypeAncode == "??" ? "" : Regex.Match(rgt, "^" + paradigm.TypeAncode + "[^а-яА-я]*([^\r]*)", RegexOptions.Multiline).Groups[1].Value));

                    for (var j = 0; j < paradigm.Count; j++)
                    {
                        Console.Write("\t\t");
                        Console.Write(paradigm.GetAccent(j) == 255 ? paradigm.GetForm(j) : paradigm.GetForm(j).Insert(paradigm.GetAccent(j) + 1, "'"));
                        Console.Write("\t");
                        Console.WriteLine(Regex.Match(rgt, "^" + paradigm.GetAncode(j) + "[^а-яА-я]*(.*)", RegexOptions.Multiline).Groups[1].Value.Replace("\r", ""));
                    }
                }
            }
        }
    }
}
