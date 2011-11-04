using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.MorphWizard;
using System.IO;
using System.Runtime.InteropServices;
using LemmatizerNET.Implement.Agramtab;
using LemmatizerNET.Files;

namespace LemmatizerNET.Implement {
	internal class MorphDict {
		private class InternalComparer : IComparer<LemmaInfoAndLemma> {
			private StringHolder _searchInfos;
			public InternalComparer(StringHolder searchInfos) {
				_searchInfos = searchInfos;
			}
			#region IComparer<CAutomAnnotationInner> Members
			public int Compare(LemmaInfoAndLemma x, LemmaInfoAndLemma y) {
				if (x == null || y==null) {
					throw new ArgumentNullException();
				}
				var intX=x as InternalLemmaInfoAndLemma;
				var intY = y as InternalLemmaInfoAndLemma;
				var strX = intX != null ? intX.Search : _searchInfos[x.LemmaStrNo];
				var strY = intY != null ? intY.Search : _searchInfos[y.LemmaStrNo];
				return string.CompareOrdinal(strX, strY);
			}
			#endregion
		}
		private class InternalLemmaInfoAndLemma : LemmaInfoAndLemma{
			private string _search;
			public string Search {
				get {
					return _search;
				}
			}
			public InternalLemmaInfoAndLemma(string search):base(0,0,0,null) {
				_search = search;
			}
		}
		private MorphAutomat _formAutomat;
		private int[] _modelsIndex;
		private byte[] _NPSs;
		private FileManager _manager;
		private List<string> _prefixes = new List<string>();
		private List<CFlexiaModel> _flexiaModels = new List<CFlexiaModel>();
		private List<CAccentModel> _accentModels = new List<CAccentModel>();
		private List<LemmaInfoAndLemma> _lemmaInfos = new List<LemmaInfoAndLemma>();
		private StringHolder _bases = new StringHolder();
		private InternalComparer _comparer;
		private string _registry;

		protected MorphAutomat FormAutomat {
			get {
				return _formAutomat;
			}
		}
		protected byte[] NPSs {
			get {
				return _NPSs;
			}
		}

		public IList<string> Prefixes {
			get {
				return _prefixes;
			}
		}
		public IList<CFlexiaModel> FlexiaModels {
			get {
				return _flexiaModels;
			}
		}
		public IList<CAccentModel> AccentModels{
			get {
				return _accentModels;
			}
		}
		public IList<string> Bases {
			get {
				return _bases;
			}
		}
		public IList<LemmaInfoAndLemma> LemmaInfos {
			get {
				return _lemmaInfos;
			}
		}
		public string Registry {
			get {
				return _registry;
			}
			protected set {
				_registry = value;
			}
		}

		protected MorphDict(InternalMorphLanguage language) {
			_formAutomat = null;
			_comparer = new InternalComparer(_bases);
		}
		protected void GetLemmaInfos(string text, int textPos, IList<AutomAnnotationInner> infos) {
			int count=infos.Count;
			for (int i = 0; i < count; i++) {
				var a = infos[i];
				var m = _flexiaModels[a.ModelNo][a.ItemNo];
				var textStartPos = textPos + _prefixes[a.PrefixNo].Length + m.PrefixStr.Length;
				var bas = _prefixes[a.PrefixNo] + text.Substring(textStartPos, text.Length - textStartPos - m.FlexiaStr.Length);
				var starts = _modelsIndex[a.ModelNo];
				var end = _modelsIndex[a.ModelNo + 1];
				var index=_lemmaInfos.BinarySearch(starts,end-starts,new InternalLemmaInfoAndLemma(bas),_comparer);
				if (index < 0) {
					index *= -1;
					index--; // Потму что указывало на след элемент
				}
				if (index >= _lemmaInfos.Count) {
					throw new IndexOutOfRangeException();
				}
				if (_bases[_lemmaInfos[index].LemmaStrNo] != bas) {
					throw new MorphException("LemmaStrNo!=Base");
				}
				a.LemmaInfoNo = index;
			}
		}
		protected void InitAutomat(MorphAutomat formAutomat) {
			if (_formAutomat != null) {
				throw new InvalidOperationException("InitAutomat already done");
			}
			if (formAutomat == null) {
				throw new ArgumentNullException("formAutomat");
			}
			_formAutomat = formAutomat;
		}
		protected void PredictBySuffix(string text, out int textOffset, int minimalPredictSuffixlen, IList<AutomAnnotationInner> infos) {
			var count = text.Length;
			for (textOffset = 1; textOffset + minimalPredictSuffixlen <= count; textOffset++) {
				_formAutomat.GetInnerMorphInfos(text, textOffset, infos);
				if (infos.Count > 0) {
					break;
				}
			}
		}
		protected bool Load(string grammarFileName, FileManager manager) {
			if (!_formAutomat.Load(MakeFName(grammarFileName, "forms_autom"),manager)) {
				return false;
			}
			var PrecompiledFile = MakeFName(grammarFileName, "annot");
			using (var file = manager.GetFile(Registry, PrecompiledFile)) {
				var reader = new BinaryReader(file, Tools.InternalEncoding);
				ReadFlexiaModels(reader, _flexiaModels);
				ReadAccentModels(reader, _accentModels);
				int Count;

				var line = Tools.ReadLine(reader);
				if (line == null) {
					return false;
				}
				int.TryParse(line, out Count);
				// add empty prefix
				_prefixes.Clear();
				_prefixes.Add("");
				for (var i = 0; i < Count; i++) {
					line = Tools.ReadLine(reader); ;
					if (line == null) {
						return false;
					}
					line = line.Trim();
					if (string.IsNullOrEmpty(line)) {
						throw new Exception("line is empty");
					}
					_prefixes.Add(line);
				}
				line = Tools.ReadLine(reader);
				if (line == null) {
					return false;
				}
				int.TryParse(line, out Count);
				if (!ReadLemmas(reader, Count)) {
					return false;
				}
				line = Tools.ReadLine(reader);
				if (line == null) {
					return false;
				}
				int.TryParse(line, out Count);
				_NPSs = reader.ReadBytes(Count); ;
				if (_NPSs.Length != Count) {
					return false;
				}
				if (_NPSs.Length != _flexiaModels.Count) {
					throw new MorphException("_NPSs.Count!=_FlexiaModels.Count");
				}
			}
			using (var file = manager.GetFile(Registry, MakeFName(grammarFileName, "bases"))) {
				_bases.ReadShortStringHolder(file);
			}
			CreateModelsIndex();
			return true;
		}
		private string MakeFName(string inpitFileName, string ext) {
			var i = inpitFileName.LastIndexOf('.'); // найти последнее расширение
			if (i >= 0) {
				inpitFileName = inpitFileName.Remove(i);
			}
			return inpitFileName + "." + ext;
		}
		private bool ReadLemmas(BinaryReader reader, int count) {
			_lemmaInfos.Clear();
			for (int i = 0; i < count; i++) {
				var model = reader.ReadInt16();
				var accent = reader.ReadInt16();
				var ancode = reader.ReadChars(Constants.CommonAncodeSize);
				var skip = reader.ReadInt16();
				var lemmaStrNo = reader.ReadInt32();
				
				_lemmaInfos.Add(new LemmaInfoAndLemma(lemmaStrNo, model, accent, ancode));
			}
			return true;
		}
		private void CreateModelsIndex() {
			if (_lemmaInfos.Count == 0) {
				return;
			}
			_modelsIndex = new int[_flexiaModels.Count + 1];
			int currentModel = _lemmaInfos[0].LemmaInfo.FlexiaModelNo;
			_modelsIndex[currentModel] = 0;
			for (var i = 0; i < _lemmaInfos.Count; i++)
				for (; currentModel < _lemmaInfos[i].LemmaInfo.FlexiaModelNo; currentModel++) {
					var debug = _lemmaInfos[i].LemmaInfo.FlexiaModelNo;
					_modelsIndex[currentModel + 1] = i;
				}
			for (; currentModel < _flexiaModels.Count; currentModel++)
				_modelsIndex[currentModel + 1] = _lemmaInfos.Count;

			for (var i = 0; i < _lemmaInfos.Count; i++) {
				int debug = _lemmaInfos[i].LemmaInfo.FlexiaModelNo;
				if (_modelsIndex[debug] > i) {
					throw new MorphException("_modelsIndex[debug] > i");
				}
				if (i >= _modelsIndex[debug + 1]) {
					throw new MorphException("i >= _modelsIndex[debug + 1");
				}
			}
		}
		private void ReadFlexiaModels(BinaryReader reader, IList<CFlexiaModel> flexiaModels) {
			var line = Tools.ReadLine(reader);
			if (line == null) {
				throw new MorphException("Cannot parse mrd file");
			}
			flexiaModels.Clear();
			int paradigm_count;
			int.TryParse(line, out paradigm_count);
			for (int num = 0; num < paradigm_count; num++) {
				line = Tools.ReadLine(reader);
				if (line == null) {
					throw new MorphException("Too few lines in mrd file");
				}
				line = line.Trim();
				var m = new CFlexiaModel();
				if (!m.ReadFromString(line)) {
					throw new MorphException("Cannot parse paradigm No " + (num + 1));
				}
				flexiaModels.Add(m);
			}
		}
		private void ReadAccentModels(BinaryReader reader, IList<CAccentModel> accentModels) {
			var line = Tools.ReadLine(reader);
			if (line == null) {
				throw new MorphException("Cannot read accent models from mrd file");
			}
			accentModels.Clear();
			int count;
			int.TryParse(line, out count);
			for (int num = 0; num < count; num++) {
				line = Tools.ReadLine(reader);
				if (line == null) {
					throw new MorphException("Too few lines in mrd file");
				}
				line = line.Trim();
				var m = new CAccentModel();
				if (!m.ReadFromString(line)) {
					throw new MorphException("Cannot parse line " + line);
				}
				accentModels.Add(m);
			}
		}
		//protected bool Save(string GrammarFileName) {
	}
}

