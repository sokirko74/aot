using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement.MorphWizard {
	internal class CMorphForm {
		private string _gramcode;
		private string _flexiaStr;
		private string _prefixStr;

		public string Gramcode {
			get {
				return _gramcode;
			}
		}
		public string FlexiaStr {
			get {
				return _flexiaStr;
			}
		}
		public string PrefixStr {
			get {
				return _prefixStr ?? "";
			}
		}
		public CMorphForm() {
		}
		public CMorphForm(string gramcode, string flexiaStr, string prefixStr) {
			_gramcode = gramcode;
			_flexiaStr = flexiaStr;
			_prefixStr = prefixStr;
			if (string.IsNullOrEmpty(_gramcode)) {
				throw new ArgumentNullException("Gramcode");
			}
		}
		public bool ReadFromString(string s) {
			var strs = s.Split('*');
			if (strs.Length < 1) {
				return false;
			}
			_flexiaStr = strs[0];
			if (strs.Length > 1) {
				_gramcode = strs[1];
			}
			if (strs.Length > 2) {
				_prefixStr = strs[2];
			}
			return true;
		}
		public override bool Equals(object obj) {
			var x =obj as CMorphForm;
			if (x != null) {
				return _gramcode == x._gramcode
					&& _flexiaStr == x._flexiaStr
					&& _prefixStr == x._prefixStr;
			}
			return base.Equals(obj);
		}
		public override string ToString() {
			var res = _flexiaStr + "*" + _gramcode;
			if (!string.IsNullOrEmpty(_prefixStr)) {
				res += "*";
				res += _prefixStr;
			}
			return res;
		}
		public override int GetHashCode() {
			return _gramcode.GetHashCode()^(_flexiaStr??"").GetHashCode()^(_prefixStr??"").GetHashCode();
		}
		

	}
}
