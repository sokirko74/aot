using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal class MorphAutomNode {
		private uint _data;
		public int ChildrenStart {
			get {
				return (int)(_data & (0x80000000 - 1));
			}
			set {
				_data = (0x80000000 & _data) | (uint)value; 
			}
		}
		public bool IsFinal {
			get {
				return (_data & 0x80000000) != 0;
			}
			set {
				if (value) {
					_data |= 0x80000000;
				} else {
					_data &= 0x80000000 - 1;
				}
			}
		}
		public int Data {
			get {
				return (int)_data;
			}
			set {
				_data = (uint)value;
			}
		}
	}
}
