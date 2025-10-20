using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ECB_v2.Loading
{
    public partial class CheckModulesWnd : Form
    {
        public CheckModulesWnd()
        {
            InitializeComponent();
        }


        void CheckModulesThread(object obj)
        {
            IntPtr dll = IntPtr.Zero;
            foreach (string i in DEFINES.CritDLLs)
            {
                this.Invoke(new MethodInvoker(() =>
                {
                    ModuleName.Text = i;
                }));
                dll = ECB_v2.dll.DLL_worker.LoadLibraryA(i);
                if (dll == IntPtr.Zero)
                {
                    MessageBox.Show(DEFINES.errors["MDERR"], "КРИТИЧЕСКАЯ ОШИБКА", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    Application.Exit();
                    return;
                }
                ECB_v2.dll.DLL_worker.FreeLibrary(dll);
            }
            this.Invoke(new MethodInvoker(() =>
            {
                Close();
            }));
        }

        private void CheckupModules(object sender, EventArgs e)
        {
            new Thread(CheckModulesThread).Start();            
        }
    }
}
