using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal class PredictTuple {
		private short _itemNo;
		private int _lemmaInfoNo;
		private byte _partOfSpeechNo;
		public short ItemNo {
			get {
				return _itemNo;
			}
			set {
				_itemNo = value;
			}
		}
		public int LemmaInfoNo {
			get {
				return _lemmaInfoNo;
			}
			set {
				_lemmaInfoNo = value;
			}
		}
		public byte PartOfSpeechNo {
			get {
				return _partOfSpeechNo;
			}
			set {
				_partOfSpeechNo = value;
			}
		}
	}
}
