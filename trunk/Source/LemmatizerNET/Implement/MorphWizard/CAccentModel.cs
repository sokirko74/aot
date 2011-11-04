using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.Agramtab;

namespace LemmatizerNET.Implement.MorphWizard {
	internal class CAccentModel {
		private byte[] _accents;
		public byte this[int index] {
			get {
				return _accents[index];
			}
		}
		public bool ReadFromString(string s) {
			var strs=s.Split(new []{';'}, StringSplitOptions.RemoveEmptyEntries);
			if (strs==null || strs.Length==0){
				return false;
			}
			_accents = new byte[strs.Length];
			for (int i = 0; i < strs.Length; i++) {
				byte val;
				if (!byte.TryParse(strs[i],out val)){
					return false;
				}
				_accents[i] = val;
			}
			return true;
		}
		public override bool Equals(object obj) {
			var item = obj as CAccentModel;
			if (item != null) {
				return Tools.ListEquals(_accents, item._accents);
			}
			return base.Equals(obj);
		}
		public override int GetHashCode() {
			int hash = 0;
			foreach (var item in _accents) {
				hash ^= item.GetHashCode();
			}
			return hash;
		}
		public override string ToString() {
			string Result="";
			for (var i = 0; i < _accents.Length; i++) {
				Result += _accents[i] + ";";
			};
			return Result;
		}
	}
}
