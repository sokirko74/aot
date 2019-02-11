using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET {
	public interface IParadigm {
		int ParadigmID { get; set; }
		int Count { get; }
		string GetAncode(int index);
		string Norm { get; }
		string GetForm(int index);
		string TypeAncode { get; }
		bool Founded { get; }
		string SrcAncode { get; }
        int SrcAccentedVowel { get; }
        int AccentModelNo { get; }
        //string SrcAccent { get; }
		string SrcNorm { get; }
		int HomonymWeight { get; }
		int WordWeight { get; }
		int HomonymWeightWithForm(int index);
		int GetAccent(int index);
		int BaseLength { get; }
		int LemmaPrefixLength { get; }
	}
}
