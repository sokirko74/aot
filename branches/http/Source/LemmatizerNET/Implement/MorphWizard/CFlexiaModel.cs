using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.Agramtab;

namespace LemmatizerNET.Implement.MorphWizard {
	internal class CFlexiaModel {
		private string _comments;
		private List<CMorphForm> _flexia=new List<CMorphForm>();
		public CMorphForm this[int index] {
			get {
				return _flexia[index];
			}
		}
		public int Count {
			get {
				return _flexia.Count;
			}
		}
		public string FirstFlex {
			get {
				if (_flexia.Count == 0) {
					throw new FieldAccessException("FirstFlex failed");
				}
				return _flexia[0].FlexiaStr;
			}
		}
		public string FirstCode {
			get {
				if (_flexia.Count == 0) {
					throw new FieldAccessException("FirstCode failed");
				}
				return _flexia[0].Gramcode;
			}
		}
		public bool has_ancode(string search_ancode) {
			foreach (var item in _flexia) {
				int index = item.Gramcode.IndexOf(search_ancode);
				if (index > 0 && index % 2 == 0) {
					return true;
				}
			}
			return false;
		}
		public bool ReadFromString(string s) {
			s = s.Replace(Constants.FlexModelCommDelim, "~");
			var tok = s.Split('~');
			if (tok.Length > 0) {
				var items = tok[0].Split(new[] { '%' }, StringSplitOptions.RemoveEmptyEntries);
				foreach (var itemStr in items) {
					var item = new CMorphForm();
					if (!item.ReadFromString(itemStr)) {
						return false;
					}
					_flexia.Add(item);
				}
			}
			if (tok.Length > 1) {
				_comments = tok[1];
			}
			return true;
		}
		public override bool Equals(object obj) {
			var l = obj as List<CMorphForm>;
			if (l!=null){
				return Tools.ListEquals(_flexia,l);
			}
			return base.Equals(obj);
		}
		public override string ToString() {
			var res = "";
			foreach(var item in _flexia) {
				res += "%" + item.ToString();
			}
			if (!string.IsNullOrEmpty(_comments)) {
				res += Constants.FlexModelCommDelim + _comments;
			}
			return res;
		}
		public override int GetHashCode() {
			int res = 0;
			foreach (var item in _flexia) {
				res ^= item.GetHashCode();
			}
			return res;
		}
		
	}
}
