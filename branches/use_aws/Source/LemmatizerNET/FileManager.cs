﻿using System;
using System.IO;
using System.Reflection;
using System.Text;
using LemmatizerNET.Files.FileSystem;
using LemmatizerNET.Implement;
using LemmatizerNET.Implement.Agramtab;

namespace LemmatizerNET {
	/// <summary>
	/// Base class for load dictionaries
	/// </summary>
	public abstract class FileManager {
		private string _registryValue;
		private string _registryPath;
		private Tools _tools;

		public FileManager()
		{
			_tools = new Tools();
		}

		private string INIFileName {
			get {
				return "/Bin/" + Constants.RMLRegistryFilename;
			}
		}
		protected abstract Stream GetFile(string name);
		public static FileManager GetDefaultFileManager() {
			var path = Assembly.GetExecutingAssembly().Location;
			path = Path.GetDirectoryName(path);
			path = Path.GetDirectoryName(path);
			return new FileSystemFileManager(path);
		}
		public static FileManager GetFileManager(string path){
			return new FileSystemFileManager(path);
		}
		public Stream GetFile(string registry, string name, int codePage) {
			string path = GetStringInnerFromTheFile(registry, codePage);
			return GetFile(path + name);
		}
		private string GetStringInnerFromTheFile(string registryPath, int codePage) {
			if (registryPath != _registryPath) {
				using (var stream = GetFile(INIFileName)) {
					using (var reader = new StreamReader(stream, _tools.InternalEncoding(codePage))) 
					{	
						var line = reader.ReadLine();
						bool find = false;
						while (line != null) {
							string[] vals = line.Split(' ', '\t');
							if (vals != null && vals.Length == 2 && vals[0] == registryPath) {
								var res = vals[1];
								if (res.StartsWith("$RML")) {
									_registryValue = res.Replace("$RML", "");
									_registryPath = registryPath;
									find = true;
									break;
								}
							}
							line = reader.ReadLine();
						}
						if (!find) {
							_registryPath = registryPath;
							_registryValue = "";
						}
					}
				}
			}
			return _registryValue;
		}
	}
}
