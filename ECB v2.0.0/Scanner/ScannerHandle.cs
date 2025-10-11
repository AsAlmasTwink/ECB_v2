using ECB_v2.dll;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace ECB_v2.Scanner
{
    internal class ScannerHandle
    {
        private string PID = null, VID = null;
        private int RequestPeriod = 200; // in ms
        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet =CharSet.Ansi)]
        public delegate IntPtr GetConnectedDevice(int maxWaitTime /*In seconds*/);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate IntPtr GetDevicePID(IntPtr device);

        [UnmanagedFunctionPointer(CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private delegate IntPtr GetDeviceVID(IntPtr device);
        
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

                IntPtr device = waitForConnect(60 * 2);
                if (device == IntPtr.Zero)
                    throw new Exception("Device not found or time out!");
                IntPtr pidPtr = GetDevPID(device),
                    vidPtr = GetDevVID(device);
                throw new Exception("Not ended the function!");
            }
            catch
            {
            }
            DLL_worker.FreeLibrary(Dll);
            return null;
        }
        private bool FindScanner(Settings st)
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
        public ScannerHandle() { }
        public bool Initialize() {  
            Settings st = new Settings(DEFINES.SETTINGS_FILE);
            if (st == null)
                Program.MemError();
            if(st.LoadFile() != Settings.ST_STATUS.SUCCESS)
            {
                return FindScanner(st);
            }
            try
            {
                Settings.ST_STATUS res = Settings.ST_STATUS.SUCCESS;
                this.PID = st.GetValue("SC_PID");
                res = res | st.status;
                this.VID = st.GetValue("SC_VID");
                res = res | st.status;
                if (res != Settings.ST_STATUS.SUCCESS)
                {
                    return FindScanner(st);
                }
                this.RequestPeriod = int.Parse(st.GetValue("SC_RPms"));
                if (st.status != Settings.ST_STATUS.SUCCESS)
                    throw new FormatException();
            }
            catch (FormatException)
            {
                this.RequestPeriod = 200;
                st.SetVal("SC_RPms", this.RequestPeriod.ToString());
            }catch
            {
                Program.ErrorMsg(DEFINES.errors["UNKNOWN"]);
            }
            return false; 
        }


        public void SetScanner(string PID, string VID, int v)
        {
            if (!string.IsNullOrEmpty(PID) && !string.IsNullOrEmpty(VID))
            {
                this.VID = VID;
                this.PID = PID;
            }
            if (v >= 50 && v <= 500)
                this.RequestPeriod = v;
            Settings st = new Settings(DEFINES.SETTINGS_FILE);
            if (st == null) Program.MemError();
            st.SetVal("SC_PID", this.PID);
            st.SetVal("SC_VID", this.VID);
            st.SetVal("SC_RPms", this.RequestPeriod.ToString());
            st.SaveSettings();
        }
        public void Free() { }
    }
}
