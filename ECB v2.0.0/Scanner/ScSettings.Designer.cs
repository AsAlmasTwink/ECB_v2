namespace ECB_v2.Scanner
{
    partial class ScSettings
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ScSettings));
            this.ActImage = new System.Windows.Forms.PictureBox();
            this.ActDesc = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.NextBtn = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.ActImage)).BeginInit();
            this.SuspendLayout();
            // 
            // ActImage
            // 
            this.ActImage.BackColor = System.Drawing.Color.Transparent;
            this.ActImage.Location = new System.Drawing.Point(72, 85);
            this.ActImage.Margin = new System.Windows.Forms.Padding(0);
            this.ActImage.Name = "ActImage";
            this.ActImage.Size = new System.Drawing.Size(300, 300);
            this.ActImage.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.ActImage.TabIndex = 0;
            this.ActImage.TabStop = false;
            // 
            // ActDesc
            // 
            this.ActDesc.AutoSize = true;
            this.ActDesc.Font = new System.Drawing.Font("Times New Roman", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.ActDesc.Location = new System.Drawing.Point(22, 9);
            this.ActDesc.MaximumSize = new System.Drawing.Size(470, 0);
            this.ActDesc.Name = "ActDesc";
            this.ActDesc.Size = new System.Drawing.Size(441, 66);
            this.ActDesc.TabIndex = 1;
            this.ActDesc.Text = "Добро пожаловать на страницу выбора считывающего устройства! Пожалуйста следуйте " +
    "указаниям в окне!";
            this.ActDesc.TextAlign = System.Drawing.ContentAlignment.TopCenter;
            // 
            // button1
            // 
            this.button1.Cursor = System.Windows.Forms.Cursors.Hand;
            this.button1.Font = new System.Drawing.Font("Times New Roman", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.button1.Location = new System.Drawing.Point(12, 399);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(142, 50);
            this.button1.TabIndex = 2;
            this.button1.Text = "Отмена";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // NextBtn
            // 
            this.NextBtn.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(192)))), ((int)(((byte)(0)))));
            this.NextBtn.Cursor = System.Windows.Forms.Cursors.Hand;
            this.NextBtn.Font = new System.Drawing.Font("Times New Roman", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.NextBtn.ForeColor = System.Drawing.SystemColors.Control;
            this.NextBtn.Location = new System.Drawing.Point(330, 399);
            this.NextBtn.Name = "NextBtn";
            this.NextBtn.Size = new System.Drawing.Size(142, 50);
            this.NextBtn.TabIndex = 3;
            this.NextBtn.Text = "Далее";
            this.NextBtn.UseVisualStyleBackColor = false;
            this.NextBtn.Click += new System.EventHandler(this.NextAct);
            // 
            // ScSettings
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(484, 461);
            this.Controls.Add(this.NextBtn);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.ActDesc);
            this.Controls.Add(this.ActImage);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ScSettings";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Выбор/поиск сканера";
            ((System.ComponentModel.ISupportInitialize)(this.ActImage)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox ActImage;
        private System.Windows.Forms.Label ActDesc;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button NextBtn;
    }
}