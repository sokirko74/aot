using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.Agramtab;

namespace LemmatizerNET.Implement {
	internal class ABCEncoder {
		private Lemmatizer _lemmatizer;
        private InternalMorphLanguage _language;
		private char _annotChar;
		private int _alphabetSize;
		protected int[] _alphabet2Code=new int[Constants.AlphabetSize];
		private int[] _code2Alphabet = new int[Constants.MaxAlphabetSize];
		private int _alphabetSizeWithoutAnnotator;
		private int[] _alphabet2CodeWithoutAnnotator = new int[Constants.AlphabetSize];
		private int[] _code2AlphabetWithoutAnnotator = new int[Constants.MaxAlphabetSize];

		public string CriticalNounLetterPack {
			get {
				return new string(_annotChar, Constants.MinimalPredictionSuffix);
			}
		}
		public InternalMorphLanguage Language {
			get {
                return _language;
			}
		}

		private static int InitAlphabet(InternalMorphLanguage language, int[] pCode2Alphabet, int[] pAlphabet2Code, char annotChar) {
			if (char.IsUpper(annotChar)) {
				throw new MorphException("annotChar is not upper");
			}
			var AdditionalEnglishChars = "'1234567890";
			var AdditionalGermanChars = "";
			var AlphabetSize = 0;
			for (var i = 0; i < Constants.AlphabetSize; i++) {
				var ch = Convert.ToChar(i);
				if (Lang.is_upper_alpha((byte)i, language)
				|| (ch == '-')
				|| (ch == annotChar)
				|| ((language == InternalMorphLanguage.morphEnglish)
						&& (AdditionalEnglishChars.IndexOf(ch) >= 0)
					)
				|| ((language == InternalMorphLanguage.morphGerman)
						&& (AdditionalGermanChars.IndexOf(ch) >= 0)
					)
				|| ((language == InternalMorphLanguage.morphURL)
					  && Lang.is_alpha((byte)i, language)
					 )) {


					pCode2Alphabet[AlphabetSize] = i;
					pAlphabet2Code[i] = AlphabetSize;
					AlphabetSize++;
				} else {
					pAlphabet2Code[i] = -1;
				}
			}
			if (AlphabetSize > Constants.MaxAlphabetSize) {
				throw new MorphException("Error! The  ABC is too large");
			}
			return AlphabetSize;
		}
		public ABCEncoder(Lemmatizer lemmatizer,InternalMorphLanguage language, char annotChar) {
            _lemmatizer = lemmatizer;
            _language = language;
			_annotChar=annotChar;
			_alphabetSize = InitAlphabet(language, _code2Alphabet, _alphabet2Code, _annotChar);
			_alphabetSizeWithoutAnnotator = InitAlphabet(language,_code2AlphabetWithoutAnnotator,_alphabet2CodeWithoutAnnotator,(char)257/* non-exeting symbol */);
			if (_alphabetSizeWithoutAnnotator + 1 != _alphabetSize) {
				throw new MorphException("_alphabetSizeWithoutAnnotator + 1 != _alphabetSize");
			}
		}
		public Lemmatizer Lemmatizer {
			get {
				return _lemmatizer;
			}
		}
		public char AnnotChar {
			get {
				return _annotChar;
			}
		}
		public bool CheckABCWithAnnotator(string wordForm) {
			var len = wordForm.Length;
			for (var i = 0; i < len; i++) {
				if (_alphabet2Code[Tools.GetByte(wordForm[i])] == -1) {
					return false;
				}
			}
			return true;
		}
		public bool CheckABCWithoutAnnotator(string wordForm) {
			var len = wordForm.Length;
			for (var i = 0; i < len; i++)
				if (_alphabet2CodeWithoutAnnotator[Tools.GetByte(wordForm[i])] == -1)
					return false;
			return true;
		}
		public int DecodeFromAlphabet(string v) {
			var len = v.Length;
			var c = 1;
			var Result = 0;
			for (var i = 0; i < len; i++) {
				Result += _alphabet2CodeWithoutAnnotator[Tools.GetByte(v[i])] * c;
				c *= _alphabetSizeWithoutAnnotator;
			};
			return Result;
		}
	}
}
