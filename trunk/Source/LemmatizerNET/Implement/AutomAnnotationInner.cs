using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal class AutomAnnotationInner {
		private ushort _modelNo;
		private ushort _itemNo;
		private ushort _prefixNo;
		private uint _lemmaInfoNo;
		private uint _weight;

		public int ParadigmId {
			get {
				return (int)((uint)((uint)(_prefixNo << 23) | _lemmaInfoNo));
			}
		}
		public int LemmaInfoNo {
			get {
				return (int)_lemmaInfoNo;
			}
			set {
				_lemmaInfoNo = (uint)value;
			}
		}
		public int Weight {
			get {
				return (int)_weight;
			}
			set {
				_weight = (uint)value;
			}
		}
		public short ItemNo {
			get {
				return (short)_itemNo;
			}
			set {
				_itemNo = (ushort)value;
			}
		}
		public short ModelNo {
			get {
				return (short)_modelNo;
			}
			set {
				_modelNo = (ushort)value;
			}
		}
		public short PrefixNo {
			get {
				return (short)_prefixNo;
			}
			set {
				_prefixNo = (ushort)value;
			}
		}
		public void SplitParadigmId(int value) {
			_prefixNo = (ushort)(value >> 23);
			_lemmaInfoNo = ((uint)value) & 0x7fffff;
		}
	}
}
