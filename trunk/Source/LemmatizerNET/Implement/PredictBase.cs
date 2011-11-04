using System;
using System.Collections.Generic;
using System.Text;
using LemmatizerNET.Implement.Agramtab;
using LemmatizerNET.Files;

namespace LemmatizerNET.Implement {
	internal class PredictBase {
		private MorphAutomat _suffixAutomat;
		private List<int> _modelFreq = new List<int>();

		public IList<int> ModelFreq {
			get {
				return _modelFreq;
			}
		}
		public PredictBase(Lemmatizer lemmatizer,InternalMorphLanguage lang) {
			_suffixAutomat = new MorphAutomat(lemmatizer,lang, Constants.MorphAnnotChar);
		}
		public void Load(string path, FileManager manager) {
			_suffixAutomat.Load(path, manager);
		}
		public bool Find(string lettId, IList<PredictTuple> res) {
			var TextLength = lettId.Length;
			var r = 0;
			var i = 0;
			for (; i < TextLength; i++) {
				var nd = _suffixAutomat.NextNode(r, lettId[i]);
				if (nd == -1) {
					break;
				}
				r = nd;
			}
			// no prediction by suffix which is less than 3
			if (i < Constants.MinimalPredictionSuffix) {
				return false;
			}
			if (r == -1) {
				throw new MorphException("r == -1");
			}
			FindRecursive(r, "", res);
			return true;
		}
		public void BuildPredict(IList<LemmaInfoAndLemma> lemmaInfos) {
			int count = lemmaInfos.Count;
			for (int i = 0; i < count; i++) {
				if (_modelFreq.Count <= lemmaInfos[i].LemmaInfo.FlexiaModelNo) {
					_modelFreq.Add(1);
				} else {
					_modelFreq[lemmaInfos[i].LemmaInfo.FlexiaModelNo]++;
				}
			}
		}
		private void FindRecursive(int r, string currPath, IList<PredictTuple> infos) {
			if (_suffixAutomat.GetNode(r).IsFinal) {
				var i = currPath.IndexOf(_suffixAutomat.AnnotChar);
				if (i < 0) {
					throw new MorphException("i<0");
				}
				var j = currPath.IndexOf(_suffixAutomat.AnnotChar, i + 1);
				if (j < 0) {
					throw new MorphException("j<0");
				}
				var k = currPath.IndexOf(_suffixAutomat.AnnotChar, j + 1);
				if (k < 0) {
					throw new MorphException("k<0");
				}
				infos.Add(new PredictTuple {
					PartOfSpeechNo = (byte)_suffixAutomat.DecodeFromAlphabet(currPath.Substring(i + 1, j - i - 1)),
					LemmaInfoNo = _suffixAutomat.DecodeFromAlphabet(currPath.Substring(j + 1, k - j - 1)),
					ItemNo = (short)_suffixAutomat.DecodeFromAlphabet(currPath.Substring(k + 1)),
				});
			}

			var count = _suffixAutomat.GetChildrenCount(r);
			var currPathSize = currPath.Length;
			var buff = new char[currPathSize + 1];
			currPath.CopyTo(0, buff, 0, currPathSize);
			for (var i = 0; i < count; i++) {
				var p = _suffixAutomat.GetChildren(r, i);
				buff[currPathSize] = Tools.GetChar(p.RelationalChar);
				FindRecursive(p.ChildNo, new string(buff), infos);
			}
		}
	}
}
