using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.Agramtab;
using LemmatizerNET.Implement.MorphWizard;
using System.IO;
using LemmatizerNET.Files;


namespace LemmatizerNET.Implement {
	internal abstract class Lemmatizer : MorphDict, ILemmatizer {
		private Set<string> _hyphenPostfixes = new Set<string>();
		private Set<string> _hyphenPrefixes = new Set<string>();
		private Statistic _statistic = new Statistic();
		private PredictBase _predict;
		private bool _loaded;
		private bool _maximalPrediction;
		private bool _useStatistic;
		private bool _allowRussianJo;
		
		public ICollection<string> HyphenPrefixes {
			get {
				return _hyphenPrefixes;
			}
		}
		public ICollection<string> HyphenPostfixes {
			get {
				return _hyphenPostfixes;
			}
		}
		public bool AllowRussianJo {
			get {
				return _allowRussianJo;
			}
			set {
				_allowRussianJo = value;
			}
		}
		public Statistic Statistic {
			get {
				return _statistic;
			}
		}
		public InternalMorphLanguage Language {
			get {
				return FormAutomat.Language;
			}
		}
		public Lemmatizer(InternalMorphLanguage language)
			: base(language) {
			_loaded = false;
			_useStatistic = false;
			_maximalPrediction = false;
			_allowRussianJo = false;
			_predict = new PredictBase(this,language);
			InitAutomat(new MorphAutomat(this,language, Constants.MorphAnnotChar));
		}
		protected abstract string FilterSrc(string src);
		
		private void ReadOptions(string fileName,FileManager manager) {
			string res;
			using (var stream = manager.GetFile(Registry, fileName)) {
				Tools.LoadFileToString(stream, out res);
			}
			var lines = res.Split(new[] { '\r', '\n' }, StringSplitOptions.RemoveEmptyEntries);
			foreach (var item in lines) {
				var r = item.Trim();
				if (r == "AllowRussianJo") {
					_allowRussianJo = true;
				}
			}
		}
		private AutomAnnotationInner ConvertPredictTupleToAnnot(PredictTuple input) {
			var node = new AutomAnnotationInner();
			node.LemmaInfoNo = input.LemmaInfoNo;
			node.ModelNo = LemmaInfos[node.LemmaInfoNo].LemmaInfo.FlexiaModelNo;
			node.Weight = 0;
			node.PrefixNo = 0;
			node.ItemNo = input.ItemNo;
			return node;
		}
		private void AssignWeightIfNeed(IList<AutomAnnotationInner> findResults) {
			for (var i = 0; i < findResults.Count; i++) {
				var a = findResults[i];
				if (_useStatistic) {
					a.Weight = _statistic.GetHomoWeight(a.ParadigmId, a.ItemNo);
				} else {
					a.Weight = 0;
				}
			}
		}
        //private void CreateDecartProduction(IList<FormInfo> results1, IList<FormInfo> results2, IList<FormInfo> results) { 
        //CreateDecartProduction(results1, results2,  results,  "") ;}
        private void CreateDecartProduction(IList<FormInfo> results1, IList<FormInfo> results2, IList<FormInfo> results, string firstPart)
        {

			results.Clear();
			for (var i = 0; i < results1.Count; i++) {
				for (var k = 0; k < results2.Count; k++) {
					var f = (FormInfo)results2[k].Clone();
                    int j = results1[i].Count;
                    while (--j > 0 && results1[i].GetForm(j) != firstPart) ;
					f.SetUserPrefix(results1[i].GetForm(j) + "-");
					results.Add(f);
				}
			}
		}
		private void PredictByDataBase(string inputWord, IList<AutomAnnotationInner> findResults, bool isCap) {
			List<PredictTuple> res = new List<PredictTuple>();
			if (CheckAbbreviation(inputWord, findResults, isCap)) {
				return;
			}
			if (CheckABC(inputWord)) {
				// if the ABC is wrong this prediction yuilds to many variants
				var chrs = inputWord.ToCharArray();
				Array.Reverse(chrs);
				var str = new string(chrs);
				_predict.Find(str, res);
			}
			var hasNps = new int[32];// assume not more than 32 different pos
			for (var i = 0; i < hasNps.Length; i++) {
				hasNps[i] = -1;
			}
			for (var j = 0; j < res.Count; j++) {
				byte partOfSpeechNo = res[j].PartOfSpeechNo;
				if (!_maximalPrediction
					&& (hasNps[partOfSpeechNo] != -1)) {
					var oldFreq = _predict.ModelFreq[findResults[hasNps[partOfSpeechNo]].ModelNo];
					var newFreq = _predict.ModelFreq[LemmaInfos[res[j].LemmaInfoNo].LemmaInfo.FlexiaModelNo];
					if (oldFreq < newFreq) {
						findResults[hasNps[partOfSpeechNo]] = ConvertPredictTupleToAnnot(res[j]);
					}
					continue;
				}
				hasNps[partOfSpeechNo] = findResults.Count;
				findResults.Add(ConvertPredictTupleToAnnot(res[j]));
			}
			if ((hasNps[0] == -1) // no noun
				|| (isCap && (Language != InternalMorphLanguage.morphGerman))) { // or can be a proper noun (except German, where all nouns are written uppercase)
				_predict.Find(FormAutomat.CriticalNounLetterPack, res);
				findResults.Add(ConvertPredictTupleToAnnot(res[res.Count - 1]));
			}
		}
		private bool IsFound(IList<FormInfo> results) {
			return results.Count > 0 && results[0].Founded;
		}
		private bool IsHyphenPrefix(string Prefix) {
			return _hyphenPrefixes.Contains(Prefix);
		}
		private bool IsHyphenPostfix(string Postfix) {
			return _hyphenPostfixes.Contains(Postfix);
		}
		private bool LemmatizeWord(string inputWord, bool cap, bool predict, IList<AutomAnnotationInner> result, bool getLemmaInfos) {
			int wordOffset = 0;
			FormAutomat.GetInnerMorphInfos(inputWord, 0, result);
			var res = result.Count > 0;
			if (result.Count == 0) {
				if (predict) {
					PredictBySuffix(inputWord, out wordOffset, 4, result);
					if (inputWord[wordOffset - 1] != '-') {
						var knownPostfixLen = inputWord.Length - wordOffset;
						var unknownPrefixLen = wordOffset;
						if (knownPostfixLen < 6) {
							if (!IsPrefix(inputWord.Substring(0, unknownPrefixLen))) {
								result.Clear();
							}
						}
					}
					for (var i = 0; i < result.Count; i++) {
						if (NPSs[result[i].ModelNo] == Constants.UnknownPartOfSpeech) {
							result.Clear();
							break;
						}
					}
				}
			}
			if (result.Count > 0) {
				if (getLemmaInfos) {
					GetLemmaInfos(inputWord, wordOffset, result);
				}

			} else {
				if (predict) {
					PredictByDataBase(inputWord, result, cap);
					for (var i = result.Count - 1; i >= 0; i--) {
						var a = result[i];
						if (FlexiaModels[a.ModelNo][a.ItemNo].FlexiaStr.Length >= inputWord.Length) {
							result.RemoveAt(i);
						}
					}
				}
			}
			return res;
		}
		private bool CheckAbbreviation(string inputWord, IList<AutomAnnotationInner> findResults, bool isCap) {
			for (var i = 0; i < inputWord.Length; i++) {
				if (!Lang.is_upper_consonant(Tools.GetByte(inputWord[i]), Language)) {
					return false;
				}
			}
			var res = new List<PredictTuple>();
			_predict.Find(FormAutomat.CriticalNounLetterPack, res);
			findResults.Add(ConvertPredictTupleToAnnot(res[0]));
			return true;
		}
		private bool IsPrefix(string prefix) {
			return Prefixes.Contains(prefix);
		}
		private IParadigmCollection CreateParadigmCollectionInner(bool bNorm, string form, bool capital, bool use_prediction) {
			var res = new List<FormInfo>();
			CreateParadigmCollection(bNorm, form, capital, use_prediction, res);
			return new ParadigmCollection(res);
		}
		private bool CreateParadigmCollection(bool isNorm, string inputWord, bool capital, bool usePrediction, List<FormInfo> result) {
			result.Clear();
			inputWord=FilterSrc(inputWord);
			inputWord = inputWord.ToUpper();
			var findResults = new List<AutomAnnotationInner>();
			bool found = LemmatizeWord(inputWord, capital, usePrediction, findResults, true);
			AssignWeightIfNeed(findResults);
			for (var i = 0; i < findResults.Count; i++) {
				var a = findResults[i];
				if (isNorm && (a.ItemNo != 0)) {
					continue;
				}
				result.Add(new FormInfo(this, a, inputWord, found));
			}
			if (!IsFound(result)) { // not found or predicted
				// if the word was not found in the dictionary 
				// and the word contains a hyphen 
				// then we should try to predict each parts of the hyphened word separately
				var results1 = new List<FormInfo>();
				var results2 = new List<FormInfo>();
				int hyph = inputWord.IndexOf('-');
				if (hyph >= 0) {
					// try to lemmatize each parts without predictions
					var firstPart = inputWord.Substring(0, hyph);
					var secondPart = inputWord.Substring(hyph + 1);
					CreateParadigmCollection(false, firstPart, capital, false, results1);
					/*
					 if the first part is equal to the second part  or the second part is an unimportant: Russian postfix
					 then we should use only the first part 
					*/
					if (firstPart == secondPart
						|| IsHyphenPostfix(secondPart)) {
						result.Clear();
						result.AddRange(results1);
					} else if (IsHyphenPrefix(firstPart)) {
						CreateParadigmCollection(false, secondPart, capital, false, results2);
						if (IsFound(results2)) {
							result.Clear();
							result.AddRange(results2);
							for (var i = 0; i < result.Count; i++) {
								result[i].SetUserPrefix(firstPart + "-");
								result[i].SetUserUnknown();
							}
						}
					} else {
						CreateParadigmCollection(false, secondPart, false, false, results2);
						if (IsFound(results1) && IsFound(results2) && firstPart.Length > 2
							&& secondPart.Length > 2) {
							// if both words were found in the dictionary
							// then we should create a decart production
                                CreateDecartProduction(results1, results2, result, firstPart);
							for (var i = 0; i < result.Count; i++) {
								result[i].SetUserUnknown();
							}
						}
					}
				}
			}
			return true;
		}
		
		
		#region ILemmatizer Members
		public bool UseStatistic {
			get {
				return _useStatistic;
			}
			set {
				_useStatistic = value;
			}
		}
		public bool MaximalPrediction {
			set {
				_maximalPrediction = value;
			}
		}
		public IParadigmCollection CreateParadigmCollectionFromNorm(string norm, bool capital, bool usePrediction) {
			return CreateParadigmCollectionInner(true, norm, capital, usePrediction);
		}
		public IParadigmCollection CreateParadigmCollectionFromForm(string form, bool capital, bool usePrediction) {
			return CreateParadigmCollectionInner(false, form, capital, usePrediction);
		}
		public IParadigm CreateParadigmFromID(int id) {
			FormInfo info = new FormInfo();
			info.AttachLemmatizer(this);
			info.SetParadigmId(id);
			return info;
		}
		public void LoadDictionariesRegistry(FileManager manager) {
			_loaded = Load(Constants.MorphMainFiles,manager);
			if (!_loaded) {
				throw new MorphException("Dictionary not loaded");
			}
			_statistic.Load(this,"l",manager);
			_useStatistic = true;
			_predict.Load(Constants.PredictBinPath,manager);
			_predict.BuildPredict(LemmaInfos);
			ReadOptions(Constants.OptionsFile,manager);
		}
		public void LoadStatisticRegistry(Subject subj, FileManager manager) {
			string prefix;
			switch (subj) {
				case Subject.Finance:
					prefix = "f";
					break;
				case Subject.Computer:
					prefix = "c";
					break;
				case Subject.Literature:
					prefix = "l";
					break;
				default:
					throw new MorphException("Unknown type of Subject");
			}
			_statistic.Load(this, prefix, manager);
		}
		public bool CheckABC(string wordForm) {
			return FormAutomat.CheckABCWithoutAnnotator(wordForm.ToUpper());
		}
		#endregion
		
	}
}