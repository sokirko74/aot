using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	class LemmatizerGerman : Lemmatizer {
		public LemmatizerGerman()
			: base(InternalMorphLanguage.morphGerman) {
			Registry = "Software\\Dialing\\Lemmatizer\\German\\DictPath";
		}
		protected override string FilterSrc(string src) {
			return src;
		}
	}
}
