using System;
using System.Collections.Generic;
using System.Text;

namespace LemmatizerNET {
	public class MorphException:Exception {
		public MorphException(string msg) : base(msg) { }
		public MorphException(string msg, Exception e) : base(msg, e) { }
	}
}
