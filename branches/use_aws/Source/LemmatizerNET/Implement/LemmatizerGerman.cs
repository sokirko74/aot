﻿using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal class LemmatizerGerman : Lemmatizer {
		public LemmatizerGerman()
			: base(InternalMorphLanguage.morphGerman) {
			Registry = "Software\\Dialing\\Lemmatizer\\German\\DictPath";
			CodePage = 1250;
		}
		protected override string FilterSrc(string src) {
			return src;
		}
	}
}
