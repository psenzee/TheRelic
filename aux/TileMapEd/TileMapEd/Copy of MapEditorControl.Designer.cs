namespace TileMapEd
{
    partial class MapEditorControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MapEditorControl));
            this.mapSplitter = new System.Windows.Forms.SplitContainer();
            this.statusStripTiles = new System.Windows.Forms.StatusStrip();
            this.tilesField = new System.Windows.Forms.ToolStripStatusLabel();
            this.statusStripMap = new System.Windows.Forms.StatusStrip();
            this.mapField = new System.Windows.Forms.ToolStripStatusLabel();
            this.controlSplitter = new System.Windows.Forms.SplitContainer();
            this.toolStripContainer = new System.Windows.Forms.ToolStripContainer();
            this.toolStrip = new System.Windows.Forms.ToolStrip();
            this.loadTilesButton = new System.Windows.Forms.ToolStripButton();
            this.loadMapButton = new System.Windows.Forms.ToolStripButton();
            this.saveMapButton = new System.Windows.Forms.ToolStripButton();
            this.newMapButton = new System.Windows.Forms.ToolStripButton();
            this.mazeStyle2Button = new System.Windows.Forms.ToolStripButton();
            this.mazeStyle3Button = new System.Windows.Forms.ToolStripButton();
            this.refreshButton = new System.Windows.Forms.ToolStripButton();
            this.tileSizeSelect = new System.Windows.Forms.ToolStripComboBox();
            this.contextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.useTileMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.selectCorToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.openTilesDialog = new System.Windows.Forms.OpenFileDialog();
            this.openMapDialog = new System.Windows.Forms.OpenFileDialog();
            this.saveMapDialog = new System.Windows.Forms.SaveFileDialog();
            this.mazeStyle1Button = new System.Windows.Forms.ToolStripButton();
            this.mazeStyle0Button = new System.Windows.Forms.ToolStripButton();
            this.tilesMapControl = new TileMapEd.MapControl();
            this.worldMapControl = new TileMapEd.MapControl();
            this.mapSplitter.Panel1.SuspendLayout();
            this.mapSplitter.Panel2.SuspendLayout();
            this.mapSplitter.SuspendLayout();
            this.statusStripTiles.SuspendLayout();
            this.statusStripMap.SuspendLayout();
            this.controlSplitter.Panel1.SuspendLayout();
            this.controlSplitter.Panel2.SuspendLayout();
            this.controlSplitter.SuspendLayout();
            this.toolStripContainer.TopToolStripPanel.SuspendLayout();
            this.toolStripContainer.SuspendLayout();
            this.toolStrip.SuspendLayout();
            this.contextMenu.SuspendLayout();
            this.SuspendLayout();
            // 
            // mapSplitter
            // 
            this.mapSplitter.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.mapSplitter.Location = new System.Drawing.Point(0, 0);
            this.mapSplitter.Margin = new System.Windows.Forms.Padding(0);
            this.mapSplitter.Name = "mapSplitter";
            // 
            // mapSplitter.Panel1
            // 
            this.mapSplitter.Panel1.Controls.Add(this.statusStripTiles);
            this.mapSplitter.Panel1.Controls.Add(this.tilesMapControl);
            this.mapSplitter.Panel1.SizeChanged += new System.EventHandler(this.tilesMapPanel_SizeChanged);
            // 
            // mapSplitter.Panel2
            // 
            this.mapSplitter.Panel2.Controls.Add(this.statusStripMap);
            this.mapSplitter.Panel2.Controls.Add(this.worldMapControl);
            this.mapSplitter.Panel2.SizeChanged += new System.EventHandler(this.worldMapPanel_SizeChanged);
            this.mapSplitter.Size = new System.Drawing.Size(640, 340);
            this.mapSplitter.SplitterDistance = 128;
            this.mapSplitter.SplitterWidth = 2;
            this.mapSplitter.TabIndex = 1;
            // 
            // statusStripTiles
            // 
            this.statusStripTiles.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.tilesField});
            this.statusStripTiles.Location = new System.Drawing.Point(0, 318);
            this.statusStripTiles.Name = "statusStripTiles";
            this.statusStripTiles.Size = new System.Drawing.Size(128, 22);
            this.statusStripTiles.TabIndex = 1;
            this.statusStripTiles.Text = "Tile Set";
            // 
            // tilesField
            // 
            this.tilesField.Name = "tilesField";
            this.tilesField.Size = new System.Drawing.Size(0, 17);
            // 
            // statusStripMap
            // 
            this.statusStripMap.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mapField});
            this.statusStripMap.Location = new System.Drawing.Point(0, 318);
            this.statusStripMap.Name = "statusStripMap";
            this.statusStripMap.Size = new System.Drawing.Size(510, 22);
            this.statusStripMap.TabIndex = 1;
            this.statusStripMap.Text = "Map";
            // 
            // mapField
            // 
            this.mapField.Name = "mapField";
            this.mapField.Size = new System.Drawing.Size(0, 17);
            // 
            // controlSplitter
            // 
            this.controlSplitter.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.controlSplitter.IsSplitterFixed = true;
            this.controlSplitter.Location = new System.Drawing.Point(0, 0);
            this.controlSplitter.Margin = new System.Windows.Forms.Padding(0);
            this.controlSplitter.Name = "controlSplitter";
            this.controlSplitter.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // controlSplitter.Panel1
            // 
            this.controlSplitter.Panel1.Controls.Add(this.toolStripContainer);
            // 
            // controlSplitter.Panel2
            // 
            this.controlSplitter.Panel2.ContextMenuStrip = this.contextMenu;
            this.controlSplitter.Panel2.Controls.Add(this.mapSplitter);
            this.controlSplitter.Panel2.SizeChanged += new System.EventHandler(this.mapsPanel_SizeChanged);
            this.controlSplitter.Size = new System.Drawing.Size(622, 400);
            this.controlSplitter.SplitterDistance = 25;
            this.controlSplitter.SplitterWidth = 2;
            this.controlSplitter.TabIndex = 2;
            // 
            // toolStripContainer
            // 
            // 
            // toolStripContainer.ContentPanel
            // 
            this.toolStripContainer.ContentPanel.Margin = new System.Windows.Forms.Padding(0);
            this.toolStripContainer.ContentPanel.Size = new System.Drawing.Size(622, 3);
            this.toolStripContainer.Location = new System.Drawing.Point(0, 0);
            this.toolStripContainer.Margin = new System.Windows.Forms.Padding(0);
            this.toolStripContainer.Name = "toolStripContainer";
            this.toolStripContainer.Size = new System.Drawing.Size(622, 28);
            this.toolStripContainer.TabIndex = 4;
            this.toolStripContainer.Text = "Tool Strip Container";
            // 
            // toolStripContainer.TopToolStripPanel
            // 
            this.toolStripContainer.TopToolStripPanel.Controls.Add(this.toolStrip);
            // 
            // toolStrip
            // 
            this.toolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this.toolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.loadTilesButton,
            this.loadMapButton,
            this.saveMapButton,
            this.newMapButton,
            this.mazeStyle0Button,
            this.mazeStyle1Button,
            this.mazeStyle2Button,
            this.mazeStyle3Button,
            this.refreshButton,
            this.tileSizeSelect});
            this.toolStrip.Location = new System.Drawing.Point(3, 0);
            this.toolStrip.Name = "toolStrip";
            this.toolStrip.Size = new System.Drawing.Size(296, 25);
            this.toolStrip.TabIndex = 0;
            // 
            // loadTilesButton
            // 
            this.loadTilesButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.loadTilesButton.Image = ((System.Drawing.Image)(resources.GetObject("loadTilesButton.Image")));
            this.loadTilesButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.loadTilesButton.Name = "loadTilesButton";
            this.loadTilesButton.Size = new System.Drawing.Size(23, 22);
            this.loadTilesButton.Text = "Load Tile Set";
            this.loadTilesButton.Click += new System.EventHandler(this.loadTiles_Click);
            // 
            // loadMapButton
            // 
            this.loadMapButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.loadMapButton.Image = ((System.Drawing.Image)(resources.GetObject("loadMapButton.Image")));
            this.loadMapButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.loadMapButton.Name = "loadMapButton";
            this.loadMapButton.Size = new System.Drawing.Size(23, 22);
            this.loadMapButton.Text = "Load Map";
            this.loadMapButton.Click += new System.EventHandler(this.loadMap_Click);
            // 
            // saveMapButton
            // 
            this.saveMapButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.saveMapButton.Image = ((System.Drawing.Image)(resources.GetObject("saveMapButton.Image")));
            this.saveMapButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.saveMapButton.Name = "saveMapButton";
            this.saveMapButton.Size = new System.Drawing.Size(23, 22);
            this.saveMapButton.Text = "Save Map";
            this.saveMapButton.Click += new System.EventHandler(this.saveMap_Click);
            // 
            // newMapButton
            // 
            this.newMapButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.newMapButton.Image = ((System.Drawing.Image)(resources.GetObject("newMapButton.Image")));
            this.newMapButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.newMapButton.Name = "newMapButton";
            this.newMapButton.Size = new System.Drawing.Size(23, 22);
            this.newMapButton.Text = "Create New Map";
            this.newMapButton.Click += new System.EventHandler(this.newMap_Click);
            // 
            // mazeStyle2Button
            // 
            this.mazeStyle2Button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mazeStyle2Button.Enabled = false;
            this.mazeStyle2Button.Image = ((System.Drawing.Image)(resources.GetObject("mazeStyle2Button.Image")));
            this.mazeStyle2Button.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mazeStyle2Button.Name = "mazeStyle2Button";
            this.mazeStyle2Button.Size = new System.Drawing.Size(23, 22);
            this.mazeStyle2Button.Text = "Maze Style C";
            this.mazeStyle2Button.Click += new System.EventHandler(this.mazeStyle2_Click);
            // 
            // mazeStyle3Button
            // 
            this.mazeStyle3Button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mazeStyle3Button.Image = ((System.Drawing.Image)(resources.GetObject("mazeStyle3Button.Image")));
            this.mazeStyle3Button.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mazeStyle3Button.Name = "mazeStyle3Button";
            this.mazeStyle3Button.Size = new System.Drawing.Size(23, 22);
            this.mazeStyle3Button.Text = "Maze Style D";
            this.mazeStyle3Button.Click += new System.EventHandler(this.mazeStyle3_Click);
            // 
            // refreshButton
            // 
            this.refreshButton.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.refreshButton.Image = ((System.Drawing.Image)(resources.GetObject("refreshButton.Image")));
            this.refreshButton.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.refreshButton.Name = "refreshButton";
            this.refreshButton.Size = new System.Drawing.Size(23, 22);
            this.refreshButton.Text = "Refresh Map View";
            this.refreshButton.Click += new System.EventHandler(this.load_Click);
            // 
            // tileSizeSelect
            // 
            this.tileSizeSelect.Alignment = System.Windows.Forms.ToolStripItemAlignment.Right;
            this.tileSizeSelect.DropDownWidth = 50;
            this.tileSizeSelect.Items.AddRange(new object[] {
            "8",
            "16",
            "32",
            "64",
            "128"});
            this.tileSizeSelect.Name = "tileSizeSelect";
            this.tileSizeSelect.Size = new System.Drawing.Size(75, 25);
            this.tileSizeSelect.Text = "32";
            this.tileSizeSelect.ToolTipText = "Tile Size Select";
            // 
            // contextMenu
            // 
            this.contextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.useTileMenuItem,
            this.selectCorToolStripMenuItem});
            this.contextMenu.Name = "contextMenuStrip";
            this.contextMenu.Size = new System.Drawing.Size(141, 48);
            this.contextMenu.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip1_Opening);
            // 
            // useTileMenuItem
            // 
            this.useTileMenuItem.Name = "useTileMenuItem";
            this.useTileMenuItem.Size = new System.Drawing.Size(140, 22);
            this.useTileMenuItem.Text = "Use This Tile";
            this.useTileMenuItem.Click += new System.EventHandler(this.useTileMenuItem_Click);
            // 
            // selectCorToolStripMenuItem
            // 
            this.selectCorToolStripMenuItem.Name = "selectCorToolStripMenuItem";
            this.selectCorToolStripMenuItem.Size = new System.Drawing.Size(140, 22);
            this.selectCorToolStripMenuItem.Text = "Select Area";
            this.selectCorToolStripMenuItem.Click += new System.EventHandler(this.selectArea_Click);
            // 
            // openTilesDialog
            // 
            this.openTilesDialog.DefaultExt = "*.png";
            this.openTilesDialog.Filter = "PNG Files|*.png|JPEG Files|*.jpg|BMP Files|*.bmp|GIF Files|*.gif|TGA Files|*.tga";
            this.openTilesDialog.Title = "Open Tile Set File";
            // 
            // openMapDialog
            // 
            this.openMapDialog.DefaultExt = "*.emp";
            this.openMapDialog.Filter = "Editor Maps|*.emp|XML Files|*.xml|MAP Files|*.map|All Files|*.*";
            this.openMapDialog.Title = "Open Map File";
            // 
            // saveMapDialog
            // 
            this.saveMapDialog.DefaultExt = "*.emp";
            this.saveMapDialog.Filter = "Editor Maps|*.emp";
            this.saveMapDialog.Title = "Save Map File";
            // 
            // mazeStyle1Button
            // 
            this.mazeStyle1Button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mazeStyle1Button.Enabled = false;
            this.mazeStyle1Button.Image = ((System.Drawing.Image)(resources.GetObject("mazeStyle1Button.Image")));
            this.mazeStyle1Button.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mazeStyle1Button.Name = "mazeStyle1Button";
            this.mazeStyle1Button.Size = new System.Drawing.Size(23, 22);
            this.mazeStyle1Button.Text = "Maze Style B";
            this.mazeStyle1Button.Click += new System.EventHandler(this.mazeStyle1_Click);
            // 
            // mazeStyle0Button
            // 
            this.mazeStyle0Button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.mazeStyle0Button.Image = ((System.Drawing.Image)(resources.GetObject("mazeStyle0Button.Image")));
            this.mazeStyle0Button.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.mazeStyle0Button.Name = "mazeStyle0Button";
            this.mazeStyle0Button.Size = new System.Drawing.Size(23, 22);
            this.mazeStyle0Button.Text = "Maze Style A";
            this.mazeStyle0Button.Click += new System.EventHandler(this.mazeStyle0_Click);
            // 
            // tilesMapControl
            // 
            this.tilesMapControl.Location = new System.Drawing.Point(0, 0);
            this.tilesMapControl.Margin = new System.Windows.Forms.Padding(0);
            this.tilesMapControl.Name = "tilesMapControl";
            this.tilesMapControl.Size = new System.Drawing.Size(128, 320);
            this.tilesMapControl.TabIndex = 0;
            // 
            // worldMapControl
            // 
            this.worldMapControl.Location = new System.Drawing.Point(0, 0);
            this.worldMapControl.Margin = new System.Windows.Forms.Padding(0);
            this.worldMapControl.Name = "worldMapControl";
            this.worldMapControl.Size = new System.Drawing.Size(492, 320);
            this.worldMapControl.TabIndex = 0;
            // 
            // MapEditorControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.controlSplitter);
            this.Margin = new System.Windows.Forms.Padding(0);
            this.Name = "MapEditorControl";
            this.Size = new System.Drawing.Size(625, 395);
            this.mapSplitter.Panel1.ResumeLayout(false);
            this.mapSplitter.Panel1.PerformLayout();
            this.mapSplitter.Panel2.ResumeLayout(false);
            this.mapSplitter.Panel2.PerformLayout();
            this.mapSplitter.ResumeLayout(false);
            this.statusStripTiles.ResumeLayout(false);
            this.statusStripTiles.PerformLayout();
            this.statusStripMap.ResumeLayout(false);
            this.statusStripMap.PerformLayout();
            this.controlSplitter.Panel1.ResumeLayout(false);
            this.controlSplitter.Panel2.ResumeLayout(false);
            this.controlSplitter.ResumeLayout(false);
            this.toolStripContainer.TopToolStripPanel.ResumeLayout(false);
            this.toolStripContainer.TopToolStripPanel.PerformLayout();
            this.toolStripContainer.ResumeLayout(false);
            this.toolStripContainer.PerformLayout();
            this.toolStrip.ResumeLayout(false);
            this.toolStrip.PerformLayout();
            this.contextMenu.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private MapControl worldMapControl;
        private System.Windows.Forms.SplitContainer mapSplitter;
        private System.Windows.Forms.SplitContainer controlSplitter;
        private MapControl tilesMapControl;
        private System.Windows.Forms.OpenFileDialog openTilesDialog;
        private System.Windows.Forms.OpenFileDialog openMapDialog;
        private System.Windows.Forms.SaveFileDialog saveMapDialog;
        private System.Windows.Forms.StatusStrip statusStripTiles;
        private System.Windows.Forms.ToolStrip toolStrip;
        private System.Windows.Forms.ContextMenuStrip contextMenu;
        private System.Windows.Forms.ToolStripContainer toolStripContainer;
        private System.Windows.Forms.ToolStripButton loadTilesButton;
        private System.Windows.Forms.ToolStripStatusLabel tilesField;
        private System.Windows.Forms.ToolStripButton loadMapButton;
        private System.Windows.Forms.ToolStripButton saveMapButton;
        private System.Windows.Forms.ToolStripButton newMapButton;
        private System.Windows.Forms.ToolStripButton refreshButton;
        private System.Windows.Forms.StatusStrip statusStripMap;
        private System.Windows.Forms.ToolStripStatusLabel mapField;
        private System.Windows.Forms.ToolStripMenuItem useTileMenuItem;
        private System.Windows.Forms.ToolStripComboBox tileSizeSelect;
        private System.Windows.Forms.ToolStripMenuItem selectCorToolStripMenuItem;
        private System.Windows.Forms.ToolStripButton mazeStyle2Button;
        private System.Windows.Forms.ToolStripButton mazeStyle3Button;
        private System.Windows.Forms.ToolStripButton mazeStyle1Button;
        private System.Windows.Forms.ToolStripButton mazeStyle0Button;
    }
}
