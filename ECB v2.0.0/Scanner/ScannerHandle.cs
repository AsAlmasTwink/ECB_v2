using ECB_v2.dll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace ECB_v2.Scanner
{
    public class ScannerError : Exception
    {
        public ScannerError() { }

        public ScannerError(string message) : base(message) { }

        public ScannerError(string message, Exception innerException) : base(message, innerException) { }
    }

    internal class ScannerHandle
    {
        private string PID = null, VID = null;
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet =CharSet.Ansi)]
        public delegate IntPtr GetConnectedDevice(int maxWaitTime /*In seconds*/);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate ushort GetDevicePID(IntPtr device);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate ushort GetDeviceVID(IntPtr device);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate void FreeDevice(IntPtr device);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate UInt64 CreateScannerObject(string pid, string vid);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate bool RegistrateObjectsReader();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate bool StartupScannersReader();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate void UnregistrateObjectsReader();

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate void DeleteScannerObject(UInt64 id);

        /// <summary>
        ///  Function wait for connection of scanner and return it PID and VID  
        /// </summary>
        /// <returns>string[2] { PID, VID }</returns>
        private string[] WaitForConnection()
        {
            
            IntPtr Dll = DLL_worker.LoadLibraryA("ECB_llc.dll");
            if (Dll == IntPtr.Zero)
            {
                Program.CheckModules();
                return new string[] { this.PID, this.VID };
            }
            try
            {
                GetConnectedDevice waitForConnect = (GetConnectedDevice)Marshal.GetDelegateForFunctionPointer(DLL_worker.GetProcAddress(Dll, "GetConnectedDevice"), typeof(GetConnectedDevice));
                GetDevicePID GetDevPID = (GetDevicePID)Marshal.GetDelegateForFunctionPointer(DLL_worker.GetProcAddress(Dll, "GetDevicePID"), typeof(GetDevicePID));
                GetDeviceVID GetDevVID = (GetDeviceVID)Marshal.GetDelegateForFunctionPointer(DLL_worker.GetProcAddress(Dll, "GetDeviceVID"), typeof(GetDeviceVID));
                FreeDevice FreeDev = (FreeDevice)Marshal.GetDelegateForFunctionPointer(DLL_worker.GetProcAddress(Dll, "FreeDevice"), typeof(FreeDevice));
                IntPtr device = waitForConnect(60 * 2);
                if (device == IntPtr.Zero)
                    throw new ScannerError("Device not found or time out!");
                ushort pidPtr =  GetDevPID(device),
                    vidPtr = GetDevVID(device);
                string[] result = new string[]{
                    pidPtr.ToString("X4"),
                    vidPtr.ToString("X4")
                };
                FreeDev(device);
                return result;
            }
            catch(ScannerError e) 
            {
                MessageBox.Show(e.Message);
            }
            finally
            {
                DLL_worker.FreeLibrary(Dll);
            }
            DLL_worker.FreeLibrary(Dll);
            return null;
        }
        private bool FindScanner()
        {
            ScSettings scSettingsWnd = new ScSettings();
            scSettingsWnd.SetWaitFunc(WaitForConnection);
            Application.Run(scSettingsWnd);
            
            if(scSettingsWnd.result != null)
            {
                if (scSettingsWnd.result.Length == 2)
                {
                    this.SetScanner(scSettingsWnd.result[0], scSettingsWnd.result[1], 200);
                    return true;
                }
            }

            return false;
        }

        private bool ScCreateConnection()
        {
            if (this.VID == null || this.PID == null)
                return false;
            return true;
        }

        public ScannerHandle() { }
        public bool Initialize() {  
            try
            {
                Settings.ST_STATUS res = Settings.ST_STATUS.SUCCESS;
                this.PID = Settings.GetValue("SC_PID");
                res = res | Settings.status;
                this.VID = Settings.GetValue("SC_VID");
                res = res | Settings.status;
                if (res != Settings.ST_STATUS.SUCCESS)
                {
                    return FindScanner();
                }
                IntPtr Dll = DLL_worker.LoadLibraryA("ECB_llc.dll");
                if(Dll == IntPtr.Zero)
                {
                    Program.CheckModules();
                    return false;
                }
            }catch
            {
                Program.ErrorMsg(DEFINES.errors["UNKNOWN"]);
            }
            return ScCreateConnection(); 
        }

        public void SetScanner(string PID, string VID, int v)
        {
            if (!string.IsNullOrEmpty(PID) && !string.IsNullOrEmpty(VID))
            {
                this.VID = VID;
                this.PID = PID;
            }
            Settings.SetVal("SC_PID", this.PID);
            Settings.SetVal("SC_VID", this.VID);
            Settings.SaveSettings();
        }

        public static bool StartUpReadCycle() {
            return false;
        }
        public void Free() { }
    }
}
