using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;


namespace ECB_v2
{
    internal static class Settings
    {
        public enum ST_STATUS
        {
          SUCCESS,
          FILE_READ_ERR,
          FILE_WRITE_ERR,
          FILE_NOT_FOUND,
          FILE_NOT_OPENED,
          KEY_NOT_FOUND,
          VALUE_NOT_FOUND,
          ANY_ERROR
        };

        private static string stFilePath;
        private static Dictionary<string, string> currentPairs = new Dictionary<string, string>();
        public  static ST_STATUS status;

        public static void SetSettingsFile(string stFile)
        {
            Settings.stFilePath = stFile;
        }

        public static ST_STATUS LoadFile()
        {
            if (!File.Exists(Settings.stFilePath)) return (Settings.status = ST_STATUS.FILE_NOT_FOUND);

            using (StreamReader sr = File.OpenText(Settings.stFilePath)) {
                string str;
                while ((str = sr.ReadLine()) != null)
                {
                    str = str.Trim();
                    if (string.IsNullOrEmpty(str)) continue;

                    string[] arr = str.Split(new char[]{'='}, 2, StringSplitOptions.RemoveEmptyEntries);
                    if (arr.Length < 2)
                        continue;
                    arr[0] = arr[0].Trim();
                    arr[1] = arr[1].Trim();
                    if (string.IsNullOrEmpty(arr[0]))
                        continue;
                    currentPairs.Add(arr[0], arr[1]);

                }
            }
            return (Settings.status = ST_STATUS.SUCCESS);

        }

        public static string GetValue(string key)
        {
            if(string.IsNullOrEmpty(key))
                return null;
            string retValue;
            try
            {
                if (Settings.currentPairs.ContainsKey(key))
                    retValue = Settings.currentPairs[key];
                else
                {
                    retValue = null;
                    Settings.status = ST_STATUS.KEY_NOT_FOUND;
                }
            }
            catch
            {
                retValue = null;
                Settings.status = ST_STATUS.ANY_ERROR;
            }
            return retValue;
        }

        public static ST_STATUS SetVal(string key, string value) {
            if(string.IsNullOrEmpty(key.Trim())|| string.IsNullOrEmpty(value.Trim())) return ST_STATUS.ANY_ERROR;
            key = key.Trim();
            value = value.Trim();
            if (Settings.currentPairs.ContainsKey(key))
            {
                Settings.currentPairs[key] = value;
            }
            else
            {
                Settings.currentPairs.Add(key, value);
            }
            return (Settings.status = ST_STATUS.SUCCESS);
        }

        public static ST_STATUS SaveSettings()
        {

            using (StreamWriter sw = File.CreateText(Settings.stFilePath)) {
                foreach (var item in Settings.currentPairs)
                {
                    string str = item.Key + "=" + item.Value;
                    str.Trim();
                    sw.WriteLine(str);
                }
            }
            return (Settings.status = ST_STATUS.SUCCESS);
        }
        
    }
}
