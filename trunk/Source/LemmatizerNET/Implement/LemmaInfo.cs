using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal class LemmaInfo:IComparable<LemmaInfo>,IEquatable<LemmaInfo> {
		private ushort _flexiaModelNo;
		private ushort _accentModelNo;
		private char[] _commonAncode;

		public string GetCommonAncodeIfCan {
			get {
				if (_commonAncode[0] == '\0') {
					return "";
				}
				return new string(_commonAncode);
			}
		}
		public short FlexiaModelNo {
			get {
				return (short)_flexiaModelNo;
			}
		}
		public short AccentModelNo {
			get {
				return (short)_accentModelNo;
			}
		}
		public LemmaInfo(short flexiaModelNo, short accentModelNo, char[] commonAncode) {
			_commonAncode=commonAncode;
			_flexiaModelNo = (ushort)flexiaModelNo;
			_accentModelNo = (ushort)accentModelNo;
		}
		public LemmaInfo() {
			_commonAncode=new[] { '\0', '\0' };
			unchecked {
				_flexiaModelNo = (ushort)Constants.UnknownParadigmNo;
				_accentModelNo = (ushort)Constants.UnknownAccentModelNo;
			}
		}
		public override string ToString() {
			return string.Format("Flexia={0};Accent={1},Ancode={2}", _flexiaModelNo, _accentModelNo, GetCommonAncodeIfCan);
		}
		#region IComparable<CLemmaInfo> Members
		public int CompareTo(LemmaInfo other) {
			if (_flexiaModelNo != other._flexiaModelNo) {
				return _flexiaModelNo.CompareTo(other._flexiaModelNo);
			}
			var res = new string(_commonAncode).CompareTo(new string(other._commonAncode));
			if (res != 0) {
				return res;
			}
			return _accentModelNo.CompareTo(other._accentModelNo);
		}
		#endregion
		#region IEquatable<CLemmaInfo> Members
		public bool Equals(LemmaInfo other) {
			return CompareTo(other) == 0;
		}
		#endregion
		
	}
}
