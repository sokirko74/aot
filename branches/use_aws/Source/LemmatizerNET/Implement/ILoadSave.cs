using System;
using System.Collections.Generic;
using System.Text;
using System.IO;

namespace LemmatizerNET.Implement {
	internal interface ILoad {
		bool Load(Stream stream);
	}
}
