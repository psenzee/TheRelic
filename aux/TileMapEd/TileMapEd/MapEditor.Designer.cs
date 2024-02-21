namespace TileMapEd
{
    partial class MapEditor
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
            this.mapEditorControl = new TileMapEd.MapEditorControl();
            this.SuspendLayout();
            // 
            // mapEditorControl
            // 
            this.mapEditorControl.Location = new System.Drawing.Point(0, 0);
            this.mapEditorControl.Margin = new System.Windows.Forms.Padding(0);
            this.mapEditorControl.Name = "mapEditorControl";
            this.mapEditorControl.Size = new System.Drawing.Size(480, 320);
            this.mapEditorControl.TabIndex = 0;
            // 
            // MapEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(480, 320);
            this.Controls.Add(this.mapEditorControl);
            this.Name = "MapEditor";
            this.ShowIcon = false;
            this.Text = "MapEditor";
            this.ResumeLayout(false);

        }

        #endregion

        private MapEditorControl mapEditorControl;

    }
}