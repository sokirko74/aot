using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET.Implement {
	internal class MorphAutomRelation {
		private uint _data;
		public int ChildNo {
			get {
				return (int)_data & 0xffffff;
			}
		}
		public byte RelationalChar {
			get {
				return (byte)(_data >> 24);
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
