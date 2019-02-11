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
                //Позволяет декодировать грамкоды
                if (word.ToLower().Contains("g") || word.Contains("\t") || word.Contains("\a")) //m_gramcodes = 0x0322630c "абажай"
                {
                    string gc = Regex.Match(word, "[А-Яа-яёЁ]+").Groups[0].Value;
                    string r = "";
                    for (int i = 0; i < gc.Length/2; i++)
                    {
                        Console.WriteLine(Regex.Match(rgt, "^" + gc.Substring(2 * i, 2) + "[^а-яА-яЕё]*(.*)", RegexOptions.Multiline).Groups[1].Value.Replace("\r", "") );
                    }
                    Console.WriteLine("");
                    continue;
                }
                var paradigmList = lem.CreateParadigmCollectionFromForm(word, false, false);
                if (paradigmList.Count == 0)
                {
                    try //Позволяет декодировать граммемы, если число вместо слова
                    {
                        string[] g = Grammems;
                        if (word.StartsWith("f")) { word = word.Substring(1); g = Flags; } //декодируем флаги
                        UInt64 gr = Convert.ToUInt64(word);
                        for (int i = g.Length - 1; i > -1; i--)
                            if (((1uL << i) & gr) > 0) Console.Write(g[i] + ","); Console.WriteLine("");
                    }
                    catch (Exception)
                    {

                    }

                    Console.WriteLine("Paradigms not found");
                    continue;
                }
                string ancodes = "";
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
                    Console.WriteLine("$type_grm = " + (paradigm.TypeAncode == "??" ? "" : Regex.Match(rgt, "^" + paradigm.TypeAncode + "[^а-яА-яёЁ]*([^\r]*)", RegexOptions.Multiline).Groups[1].Value));
                    ancodes += paradigm.TypeAncode;
                    for (var j = 0; j < paradigm.Count; j++)
                    {
                        ancodes += paradigm.GetAncode(j);
                        Console.Write("\t\t");
                        Console.Write(paradigm.GetAccent(j) == 255 ? paradigm.GetForm(j) : paradigm.GetForm(j).Insert(paradigm.GetAccent(j) + 1, "'"));
                        Console.Write("\t");
                        Console.WriteLine(Regex.Match(rgt, "^" + paradigm.GetAncode(j) + "[^а-яА-яЕё]*(.*)", RegexOptions.Multiline).Groups[1].Value.Replace("\r", ""));
                    }
                    }
            }
        }
        static string[] Grammems = { 
					// 0..1
				   	"мн","ед",
					// 2..8
					"им","рд","дт","вн","тв","пр","зв",
					// род 9-12
					"мр","жр","ср","мр-жр",
					// 13..15
					"нст","буд","прш",
					// 16..18
					"1л","2л","3л",	
					// 19
					"пвл",
					// 20..21
					"од","но",	
					// 22
					"сравн",
					// 23..24
					"св","нс",	
					// 25..26
					"нп","пе",
					// 27..28
					"дст","стр",
					// 29-31
					"0", "аббр", "отч",
					// 32-33
					"лок", "орг",
					// 34-35
					"кач", "дфст",
					// 36-37 (наречия)
					"вопр", "указат",
					// 38..39
					"имя","фам",
					// 40
					"безл",
					// 41,42
					"жарг", "опч",
					// 43,44,45
					"разг", "притяж", "арх",
					// для второго родительного и второго предложного
					"2",
					"поэт", "проф",
					"прев", "полож"
					};

        static string[] Flags = { "",
            "fl_punct", // = 1 << 1,
            "fl_russian_odin", // = 1 << 2, // for Russian //  может заканчиваться на 01, 21,31,41,51,61,71,81,91, но не на 11
            "fl_oborot1", // = 1 << 3,
            "fl_oborot2", // = 1 << 4,
            "fl_in_oborot", // = 1 << 5,
            "fl_le", // = 1 << 6, // for Russian
            "fl_ile", // = 1 << 7,
            "fl_ranknoun", // = 1 << 8,
            "fl_digit", // = 1 << 9,
            "fl_ambiguous", // = 1 << 10,
            "fl_comma", // = 1 << 11,
            "fl_small_number", // = 1 << 12, // for Russian //  может заканчиваться на 2, 3 или 4
            "fl_dg_ch", // = 1 << 13,
            "fl_standard_param_abbr", // = 1 << 14,
            "fl_fullstop", // = 1 << 15,
            "fl_morph_noun", // = 1 << 16,
            "fl_syn_noun", // = 1 << 17,
            "fl_relative_clause", // = 1 << 18,
            "fl_is_russian", // = 1 << 19,
            "fl_can_subdue_infinitive", // = 1 << 20,
            "fl_can_subdue_instr", // = 1 << 21,
            "fl_noun_has_adj_declination" // = 1 << 22

        };
    }
}
