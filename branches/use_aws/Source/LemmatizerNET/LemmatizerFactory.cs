using System;
using LemmatizerNET.Implement;


namespace LemmatizerNET {
	public static class LemmatizerFactory {
		public static ILemmatizer Create(MorphLanguage lang) {
			switch (lang) {
				case MorphLanguage.English:
					return new LemmatizerEnglish();
				case MorphLanguage.German:
					return new LemmatizerGerman();
				case MorphLanguage.Russian:
					return new LemmatizerRussian();
				default:
					throw new MorphException("Unknown language");
			}
		}
	}
}
