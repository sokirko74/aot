using System;
using System.IO;
using LemmatizerNET.Implement;

namespace LemmatizerNET.Files.FileSystem {
	internal class FileSystemFileManager:FileManager {
		private string _path;
		public FileSystemFileManager(string path) {
			_path = path.Replace('\\', '/');
			if (_path.EndsWith("/")){
				_path=_path.Remove(path.Length-1);
			}
		}
		protected override Stream GetFile(string name) {
			return new FileStream(_path+name, FileMode.Open, FileAccess.Read);
		}
	}
}
