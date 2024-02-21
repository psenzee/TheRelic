using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;

namespace TileMapEd
{
    partial class MapTabControl
    {
        public MouseClickInfo MouseLeft
        {
            get { return GetActiveMapControl().MouseLeft; }
        }

        public MouseClickInfo MouseRight
        {
            get { return GetActiveMapControl().MouseRight; }
        }

        public Point MouseAt
        {
            get { return GetActiveMapControl().MouseAt; }
        }

        public Point ClickAt
        {
            get { return GetActiveMapControl().ClickAt; }
        }

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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            tabs       = new ArrayList();
            maps       = new ArrayList();
            tabControl = new System.Windows.Forms.TabControl();

            tabControl.SuspendLayout();
            this.SuspendLayout();
            // 
            // tabControl
            // 
            tabControl.Location = new System.Drawing.Point(0, 0);
            tabControl.Name = "TabControl";
            tabControl.SelectedIndex = 0;
            tabControl.Size = new System.Drawing.Size(150, 150);
            tabControl.TabIndex = 0;

            // 
            // MapTabControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.AutoSizeMode = System.Windows.Forms.AutoSizeMode.GrowAndShrink;
            this.AutoSize = true;
            this.Controls.Add(this.tabControl);
            this.Name = "MapTabControl";

            AddMapTab();
        }

        #endregion

        public MapControl GetMapControl(int index)
        {
            if (index >= maps.Count)
                return null;
            return (MapControl)maps[index];
        }

        public MapControl GetActiveMapControl()
        {
            return GetMapControl(tabControl.SelectedIndex);
        }

        public void SetMap(ITileMapView map)
        {
            GetActiveMapControl().SetMap(map);
        }

        public void SetEditMode(MapControl.EditMode mode)
        {
            foreach (MapControl map in maps)
                //GetActiveMapControl().SetEditMode(mode);
                map.SetEditMode(mode);
        }

        public MapControl.EditMode GetEditMode()
        {
            return GetActiveMapControl().GetEditMode();
        }

        public void SetFillTile(int id)
        {
            GetActiveMapControl().SetFillTile(id);
        }

        public void SetTileMap(ITileMap map)
        {
            GetActiveMapControl().SetTileMap(map);
        }

        public ITileMap GetTileMap()
        {
            return GetActiveMapControl().GetTileMap();
        }

        // Event Handlers
        protected override void OnSizeChanged(EventArgs e)
        {
            tabControl.Size = this.ClientSize;
            foreach (TabPage tab in tabs)
                tab.Size = this.ClientSize;
            foreach (MapControl map in maps)
                map.Size = this.ClientSize;
          //GetActiveMapControl().Size = this.ClientSize; // ChangeSize(e);
            base.OnSizeChanged(e);
        }

        private void view_CellClick(Object sender, DataGridViewCellEventArgs e)
        {
            GetActiveMapControl().view_CellClick(sender, e);
        }

        public void SetFirstDisplayedColumn(int index)
        {
            foreach (MapControl map in maps)
                map.SetFirstDisplayedColumn(index);
        }

        public void SetFirstDisplayedRow(int index)
        {
            foreach (MapControl map in maps)
                map.SetFirstDisplayedRow(index);
        }

        public void AddMapTab()
        {
            this.SuspendLayout();
            int index = tabs.Count;
            MapControl mapControl = new MapControl();
            mapControl.Location = new System.Drawing.Point(0, 0);
            mapControl.Margin = new System.Windows.Forms.Padding(0);
            mapControl.Name = "Map Control" + index;
            mapControl.Size = new System.Drawing.Size(492, 320);
            mapControl.SetEditMode(MapControl.EditMode.Fill);
            mapControl.TabIndex = index;
            mapControl.SetMapTabControl(this);
            System.Windows.Forms.TabPage page = new System.Windows.Forms.TabPage();
            page.SuspendLayout();
            page.Controls.Add(mapControl);
            page.Location = new System.Drawing.Point(4, 22);
            page.Name = "Map Tab " + index;
            page.Padding = new System.Windows.Forms.Padding(3);
            page.Size = new System.Drawing.Size(142, 124);
            page.TabIndex = index;
            page.Text = "Map " + index;
            page.UseVisualStyleBackColor = true;
            tabs.Add(page);
            maps.Add(mapControl);
            tabControl.SuspendLayout();
            tabControl.Controls.Add(page);
            page.ResumeLayout(false);
            page.PerformLayout();
            tabControl.ResumeLayout(false);
            tabControl.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();
        }

        private System.Windows.Forms.TabControl tabControl;
        private ArrayList                       maps;
        private ArrayList                       tabs;
    }
}
