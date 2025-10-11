using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ECB_v2.resources
{
    internal class Resources
    {
        static string folder;
        static string imageFolder;

        private static string LoadParametr(Settings st, string paramName, string paramDefault) {
            string result;
            result = st.GetValue(paramName);

            if (st.status != Settings.ST_STATUS.SUCCESS)
            {
                result = paramDefault;
                st.SetVal(paramName, paramDefault);
                st.SaveSettings();
            }
            return result;
        }
        public static bool LoadResources()
        {
            Settings st = new Settings(DEFINES.SETTINGS_FILE);
            st.LoadFile();

            try
            {
                folder = LoadParametr(st, "RES", DEFINES.RESOURCE_DEF);
                imageFolder = LoadParametr(st, "IMG", DEFINES.RESOURCE_IMG_DEF);
            }
            catch
            {
                return false;
            }
            return true;
        }

        public static bool LoadResourceIMG(PictureBox pictureBox, string name)
        {
            
            string path = folder + imageFolder + name;

            try
            {
                pictureBox.Load(path);
            }
            catch{
                return false;
            }
            return true;
        }

        public static Bitmap LoadResourceIMG(string name)
        {
            
            string path = folder + imageFolder + name;

            try
            {
                Bitmap nBitMap = new Bitmap(path);
                if (nBitMap == null)
                    Program.MemError();
            }
            catch{
            }
            return null;
        }
    }
}
