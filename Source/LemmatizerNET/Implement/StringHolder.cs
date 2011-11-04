using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using LemmatizerNET.Implement.Agramtab;

namespace LemmatizerNET.Implement {
	internal class StringHolder : List<string> {
		internal void ReadShortStringHolder(Stream file) {
			Clear();
			var reader = new BinaryReader(file, Tools.InternalEncoding);
			var count = reader.ReadInt32();
			for (int i = 0; i < count; i++) {
				var stringLen = reader.ReadByte();
				var chrs = reader.ReadChars(stringLen);
				var empty = reader.ReadByte();
				Add(new string(chrs));
			}
		}
	}
}
