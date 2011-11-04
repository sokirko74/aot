using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal static class Constants {
		public const string MorphMainFiles = "morph.bin";
		public const string PredictBinPath = "npredict.bin";
		public const string OptionsFile = "morph.options";
		public const string HomoweightBinPath = "homoweight.bin";
		public const string WordweightBinPath = "wordweight.bin";
		public const string RMLRegistryFilename = "rml.ini";
		public const string FlexModelCommDelim = "q//q";
		public const char MorphAnnotChar = '+';
		public const byte UnknownPartOfSpeech = 0xff;
		public const byte UnknownAccent = 0xff;	// не менять - уже проставлено в mrd
		public const short UnknownParadigmNo = -2;
		public const short UnknownAccentModelNo = -2;
		public const int MinimalPredictionSuffix = 3;
		public const int ChildrenCacheSize = 1000;
		public const int AlphabetSize = 256;
		public const int ErrorParadigmId = -1;
		public const int CommonAncodeSize = 2;
		public const int MaxAlphabetSize = 54;

		

	}
}
