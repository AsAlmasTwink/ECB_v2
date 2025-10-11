using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;


namespace ECB_v2
{
    internal class Settings
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
        
        private string stFilePath;
        private Dictionary<string, string> currentPairs = new Dictionary<string, string>();
        public  ST_STATUS status;

        public Settings(string stFilePath) { this.stFilePath = stFilePath; }

        public ST_STATUS LoadFile()
        {
            if (!File.Exists(this.stFilePath)) return (this.status = ST_STATUS.FILE_NOT_FOUND);

            using (StreamReader sr = File.OpenText(this.stFilePath)) {
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
            return (this.status = ST_STATUS.SUCCESS);

        }

        public string GetValue(string key)
        {
            if(string.IsNullOrEmpty(key))
                return null;
            string retValue;
            try
            {
                if (this.currentPairs.ContainsKey(key))
                    retValue = this.currentPairs[key];
                else
                {
                    retValue = null;
                    this.status = ST_STATUS.KEY_NOT_FOUND;
                }
            }
            catch
            {
                retValue = null;
                this.status = ST_STATUS.ANY_ERROR;
            }
            return retValue;
        }

        public ST_STATUS SetVal(string key, string value) {
            if(string.IsNullOrEmpty(key.Trim())|| string.IsNullOrEmpty(value.Trim())) return ST_STATUS.ANY_ERROR;
            key = key.Trim();
            value = value.Trim();
            if (this.currentPairs.ContainsKey(key))
            {
                this.currentPairs[key] = value;
            }
            else
            {
                this.currentPairs.Add(key, value);
            }
            return (this.status = ST_STATUS.SUCCESS);
        }

        public ST_STATUS SaveSettings()
        {

            using (StreamWriter sw = File.CreateText(this.stFilePath)) {
                foreach (var item in this.currentPairs)
                {
                    string str = item.Key + "=" + item.Value;
                    str.Trim();
                    sw.WriteLine(str);
                }
            }
            return (this.status = ST_STATUS.SUCCESS);
        }
        
    }
}
