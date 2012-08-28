using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.MorphWizard;
using LemmatizerNET.Implement.Agramtab;

namespace LemmatizerNET.Implement {
	internal class FormInfo: IParadigm, ICloneable {
		private bool _prefixesWereCut;
		private bool _flexiaWasCut;
		private string _userPrefix;
		private AutomAnnotationInner _innerAnnot;
		private Lemmatizer _parent;
		private string _inputWordBase;
		private bool _found;

		private LemmaInfoAndLemma LemmaInfo {
			get {
				return _parent.LemmaInfos[_innerAnnot.LemmaInfoNo];
			}
		}
		private CFlexiaModel FlexiaModel {
			get {
				return _parent.FlexiaModels[LemmaInfo.LemmaInfo.FlexiaModelNo];
			}
		}
		private bool IsValid {
			get {
				return _parent != null && _innerAnnot.LemmaInfoNo != -1;
			}
		}

		public string InputWordBase {
			get {
				return _inputWordBase;
			}
		}
		// returns the weight of this word 
		public int SrcAccentedVowel {
			get {
				return GetAccent(_innerAnnot.ItemNo);
			}
		}
        public int AccentModelNo
        {
            get
            {
                return LemmaInfo.LemmaInfo.AccentModelNo;
            }
        }
		public char LemSign {
			get {
				return _found ? '+' : '-';
			}
		}

		public FormInfo() {
			_found = true;
		}
		public FormInfo(Lemmatizer parent, AutomAnnotationInner a, string inputWordForm, bool found) {
			_innerAnnot = a;
			_parent = parent;
			_found = found;
			_inputWordBase = inputWordForm;
			var m = FlexiaModel[a.ItemNo];
			var flexLength = m.FlexiaStr.Length;

			//  It can be so( if CLemmatizer::PredictByDataBase was used) that 
			//  the flexion  is not suffix of m_InputWordBase, but only part of  it.
			//  If so, then we cannot generate paradigm, since the current form cannot be 
			//  divided into two parts: the base and a known flexion.

			if (_found
				|| ((_inputWordBase.Length >= flexLength)
				&& (_inputWordBase.Substring(_inputWordBase.Length - flexLength) == m.FlexiaStr))) {
				_flexiaWasCut = true;

				var rempos = _inputWordBase.Length - m.FlexiaStr.Length;
				if (rempos < _inputWordBase.Length) {
					_inputWordBase = _inputWordBase.Remove(rempos);
				}
			} else {
				_flexiaWasCut = false;
			}
			var lemmPrefix = _parent.Prefixes[_innerAnnot.PrefixNo];
			if (_found
				|| ((_inputWordBase.Substring(0, lemmPrefix.Length) == lemmPrefix)
				&& (_inputWordBase.Substring(lemmPrefix.Length, m.PrefixStr.Length) == m.PrefixStr))) {
				_inputWordBase = _inputWordBase.Remove(0, lemmPrefix.Length + m.PrefixStr.Length);
				_prefixesWereCut = true;
			} else {
				_prefixesWereCut = false;
			}
		}
		public void AttachLemmatizer(Lemmatizer parent) {
			_parent = parent;
		}
		public bool SetParadigmId(int newVal) {
			if (_parent == null) {
				throw new MorphException("_parent == null");
			}
			var a = new AutomAnnotationInner();
			a.SplitParadigmId(newVal);
			if (a.LemmaInfoNo > _parent.LemmaInfos.Count) {
				return false;
			}
			if (a.PrefixNo > _parent.Prefixes.Count) {
				return false;
			}
			a.ItemNo = 0;
			a.Weight = _parent.Statistic.GetHomoWeight(a.ParadigmId, 0);
			a.ModelNo = _parent.LemmaInfos[a.LemmaInfoNo].LemmaInfo.FlexiaModelNo;
			_innerAnnot = a;

			_prefixesWereCut = true;
			_flexiaWasCut = true;
			_found = true;
			_inputWordBase = SrcNorm;
			_inputWordBase=_inputWordBase.Remove(_inputWordBase.Length - FlexiaModel.FirstFlex.Length);

			return true;
		}
		// returns the weight of a word form 
		public void SetUserPrefix(string userPrefix) {
			_userPrefix = userPrefix;
		}
		public void SetUserUnknown() {
			_found = false;
		}
		public override string ToString() {
			string result = LemSign
				+ TypeAncode
				+ " "
				+ GetForm(0)
				+ " "
				+ SrcAncode;
			if (_found) {
				return result + string.Format(" {0} {1}", ParadigmID, HomonymWeight);
			} else {
				return result + " -1 0";
			}
		}
		#region IParadigm Members
		public int ParadigmID {
			get {
				if (!IsValid) {
					return -1;
				}
				if (!_found) {
					return Constants.ErrorParadigmId;
				} else {
					return _innerAnnot.ParadigmId;
				}
			}
			set {
				SetParadigmId(value);
			}
		}
		public int Count {
			get {
				if (!IsValid) {
					return 0;
				}
				return FlexiaModel.Count;
			}
		}
		public string GetAncode(int index) {
			if (!IsValid) {
				return "";
			}
			return FlexiaModel[index].Gramcode.Substring(0, 2);
		}
		public string Norm {
			get {
				return GetForm(0);
			}
		}
		public string GetForm(int index) {
			if (!IsValid) {
				return "";
			}
			var f = FlexiaModel[index];
			var result = _userPrefix ?? "";
			if (_prefixesWereCut) {
				result += _parent.Prefixes[_innerAnnot.PrefixNo] + f.PrefixStr;
			}
			result += _inputWordBase;
			if (_flexiaWasCut) {
				result += f.FlexiaStr;
			}
			// "m_pParent->m_Prefixes[m_InnerAnnot.m_PrefixNo]" is a prefix thta was ascribed to the whole paradigm
			// "F.m_PrefixStr" is a prefix that is ascribed to the current word form
			return result;
		}
		public string TypeAncode { // Same as CommonAncode
			get {
				if (!IsValid) {
					return "??";
				}
				var c = LemmaInfo.LemmaInfo.GetCommonAncodeIfCan;
				if (!string.IsNullOrEmpty(c)) {
					return c;
				} else {
					return "??";
				}
			}
		}
		public bool Founded {
			get {
				return _found;
			}
		}
		public string SrcAncode {
			get {
				if (!IsValid) {
					return "";
				}
				return FlexiaModel[_innerAnnot.ItemNo].FlexiaStr + "*" + FlexiaModel[_innerAnnot.ItemNo].Gramcode;
			}
		}
		public string SrcNorm {
			get {
				if (!IsValid) {
					return "";
				}
				var result = _parent.Bases[LemmaInfo.LemmaStrNo];
				result += FlexiaModel.FirstFlex;
				return result;
			}
		}
		public int HomonymWeight {
			get {
				if (!IsValid) {
					return 0;
				}
				if (!_found) {
					return 0;
				}
				return _innerAnnot.Weight;
			}
		}
		public int WordWeight {
			get {
				if (!IsValid) {
					return 0;
				}
				return _parent.Statistic.GetWordWeight(ParadigmID);
			}
		}
		public int HomonymWeightWithForm(int index) {
			if (!IsValid) {
				return 0;
			}
			return _parent.Statistic.GetHomoWeight(ParadigmID, index);
		}
		public int GetAccent(int index) {
			if (!_found) {
				return Constants.UnknownAccent;
			}
			if (!IsValid) {
				return Constants.UnknownAccent;
			}
			if (LemmaInfo.LemmaInfo.AccentModelNo == Constants.UnknownAccentModelNo) {
				return Constants.UnknownAccent;
			}
			var backVowelNo = _parent.AccentModels[LemmaInfo.LemmaInfo.AccentModelNo][index];
			return Tools.TransferReverseVowelNoToCharNo(GetForm(index).ToLower(), backVowelNo, _parent.Language);
		}
		public int BaseLength {
			get {
				return _inputWordBase.Length;
			}
		}
		public int LemmaPrefixLength {
			get {
				if (!IsValid) {
					return 0;
				}
				if (!_found) {
					return 0;
				}
				return _parent.Prefixes[_innerAnnot.PrefixNo].Length;
			}
		}
		#endregion
		#region ICloneable Members
		public object Clone() {
			var res = new FormInfo
			{
				_flexiaWasCut = _flexiaWasCut,
				_found = _found,
				_prefixesWereCut = _prefixesWereCut,
				_innerAnnot = _innerAnnot,
				_inputWordBase = _inputWordBase,
				_parent = _parent,
				_userPrefix = _userPrefix
			};
			return res;
		}
		#endregion
	}
}
