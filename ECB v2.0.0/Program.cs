using System;
using System.Collections.Generic;
using System.Windows.Forms;
using ECB_v2.Scanner;
using ECB_v2.resources;

namespace ECB_v2
{
    public static class DEFINES
    {
        public const string SETTINGS_FILE = "ECB_settings.txt";
        public const string RESOURCE_DEF = "resources/";
        public const string RESOURCE_IMG_DEF = "img/";
        public static readonly Dictionary<string, string> errors = new Dictionary<string, string> { 
            ["MDERR"] = "ОШИБКА! Критически важные модули программы не установлены или повреждены! Переустановите приложение!",
            ["MEM"] = "ОШИБКА! Оперативная память не выделена! Проверьте соответствие системы требованиям программы!",
            ["UNKNOWN"] = "ОШИБКА! Оперативная память не выделена! Проверьте соответствие системы требованиям программы!",
            ["STERR"] = "ОШИБКА! Оперативная память не выделена! Проверьте соответствие системы требованиям программы!"
        };

        public static readonly Dictionary<string, string> warns = new Dictionary<string, string>
        {
            ["SCNOTFOUND"] = "ВНИМАНИЕ! Сканнер не обнаружен! Возможны сбои в работе программы!"
        };
        public static readonly string[] CritDLLs = {
            "ECB_llc.dll"
        };
    }

    internal static class Program
    {
        public static ScannerHandle hScanner;
        public static void CheckModules()
        {
            Application.Run(new Loading.CheckModulesWnd());
        }

        public static void WarnMsg(string text)
        {
            MessageBox.Show(text, "ВНИМАНИЕ", MessageBoxButtons.OK, MessageBoxIcon.Warning);
        }

        public static void ErrorMsg(string text)
        {
            MessageBox.Show(text, "КРИТИЧЕСКАЯ ОШИБКА", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        public static void MemError()
        {
            ErrorMsg(DEFINES.errors["MEM"]);
            Application.Exit();
        }

        public static void Restart() {
            Application.Restart();
        }
        /// <summary>
        /// Главная точка входа для приложения.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            CheckModules();

            Settings.SetSettingsFile(DEFINES.SETTINGS_FILE);
            switch (Settings.LoadFile())
            {
                case Settings.ST_STATUS.FILE_NOT_FOUND: {
                        Settings.SetVal("CtrlNum", "0xffffffff");
                        Settings.SaveSettings();

                }break;
                case Settings.ST_STATUS.SUCCESS:break;
                default:
                    Program.ErrorMsg(DEFINES.errors["STERR"]); Application.Exit(); break;
            }
            Resources.LoadResources();
            hScanner = new ScannerHandle();
            if(hScanner == null)
            {
                WarnMsg(DEFINES.warns["SCNOTFOUND"]);
                MemError();
                return;
            }

            if (!hScanner.Initialize())
                WarnMsg(DEFINES.warns["SCNOTFOUND"]);

            Application.Run(new Start());
        }
    }
}
