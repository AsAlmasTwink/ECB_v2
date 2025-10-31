using ECB_v2.resources;
using System;
using System.Windows.Forms;

namespace ECB_v2.Scanner
{
    public partial class ScSettings : Form
    {
        public delegate string[] WaitForConnect();
        private WaitForConnect waitFunc;
        public string[] result = null;
        private int ActNum = 0;
        public ScSettings()
        {
            InitializeComponent();
            resources.Resources.LoadResourceIMG(this.ActImage,"Hi.png");
            this.FormBorderStyle = FormBorderStyle.Fixed3D;
        }

        public void SetWaitFunc(WaitForConnect func)
        {
            this.waitFunc = func;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            this.result = null;
            this.Close();
        }

        private void NextAct(object sender, EventArgs e)
        {
            ActNum++;

            switch (ActNum)
            {
                case 1: {
                        this.ActDesc.Text = "Отключите устройство от компьютера!";
                        Resources.LoadResourceIMG(this.ActImage, "disconnected.png");
                    } break;
                case 2: {
                        this.ActDesc.Text = "Нажмите далее, а затем подключите устройство к компьютеру!";
                        Resources.LoadResourceIMG(this.ActImage, "connected.png");
                    } break;
                case 3: {
                        this.ActDesc.Text = "Ожидается подключение... Идёт поиск...";
                        Resources.LoadResourceIMG(this.ActImage, "search.png");
                        this.Hide();
                        this.result = waitFunc();
                        this.Show();
                        NextAct(sender, e);
                    }
                    break;
                case 4: {
                        this.NextBtn.Text = "ГОТОВО";
                        if (this.result != null)
                        {
                            if (this.result.Length == 2)
                            {
                                this.ActDesc.Text = "Найдено!";
                                Resources.LoadResourceIMG(this.ActImage, "connected.png");
                                return;
                            }
                        }
                        this.ActDesc.Text = "Не найдено!";
                        Resources.LoadResourceIMG(this.ActImage, "disconnected.png");
                    }
                    break;
                default:
                    this.Close(); break;
            }
        }
    }
}
