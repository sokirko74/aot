using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal class LemmaInfoAndLemma : IComparable<LemmaInfoAndLemma> {
		private int _lemmaStrNo;
		private LemmaInfo _lemmaInfo;
		public int LemmaStrNo {
			get {
				return _lemmaStrNo;
			}
		}
		public LemmaInfo LemmaInfo {
			get {
				return _lemmaInfo;
			}
		}
		public LemmaInfoAndLemma(int lemmaStrNo, short flexiaModelNo, short accentModelNo, char[] commonAncode) {
			_lemmaStrNo = lemmaStrNo;
			_lemmaInfo = new LemmaInfo(flexiaModelNo, accentModelNo, commonAncode);
		}
		public override string ToString() {
			return string.Format("StrNo={0};Info=[{1}]", _lemmaStrNo, _lemmaInfo);
		}
		#region IComparable<CLemmaInfoAndLemma> Members
		public int CompareTo(LemmaInfoAndLemma other) {
			if (_lemmaInfo.FlexiaModelNo != other._lemmaInfo.FlexiaModelNo) {
				return _lemmaInfo.FlexiaModelNo.CompareTo(other._lemmaInfo.FlexiaModelNo);
			}
			return _lemmaStrNo.CompareTo(other._lemmaStrNo);
		}
		#endregion
	}
}
