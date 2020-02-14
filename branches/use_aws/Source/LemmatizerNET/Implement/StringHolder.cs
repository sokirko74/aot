using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using LemmatizerNET.Implement.Agramtab;

namespace LemmatizerNET.Implement {
	internal class StringHolder : List<string> {
		private Tools _tools;
		public StringHolder()
		{
			_tools = new Tools();
		}
		internal void ReadShortStringHolder(Stream file, int codePage) {
			Clear();
			var reader = new BinaryReader(file, _tools.InternalEncoding(codePage));
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
