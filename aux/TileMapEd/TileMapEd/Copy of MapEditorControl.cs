using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.IO;
using System.Text;
using System.Xml;
using System.Windows.Forms;

using utility;

namespace TileMapEd
{
    public partial class MapEditorControl : UserControl
    {
        public MapEditorControl()
        {
            InitializeComponent();
            tileSize = new Size(32, 32);
            tilesMapControl.SetEditMode(MapControl.EditMode.Select);
            worldMapControl.SetEditMode(MapControl.EditMode.Fill);
            tilesMapControl.fillChanged = OnFillChanged;
        }

        // Event Handlers
        protected override void OnSizeChanged(EventArgs e)
        {
            controlSplitter.Size = this.ClientSize;
            base.OnSizeChanged(e);
        }

        private void OnFillChanged(MapControl sender, int tile)
        {
            worldMapControl.SetFillTile(tile);
        }

        private void worldMapPanel_SizeChanged(object sender, EventArgs e)
        {
            Size size = mapSplitter.Panel2.ClientSize;
            worldMapControl.ClientSize = new Size(size.Width, size.Height - 15);
        }

        private void mapsPanel_SizeChanged(object sender, EventArgs e)
        {
            mapSplitter.ClientSize = controlSplitter.Panel2.ClientSize;
        }

        private void tilesMapPanel_SizeChanged(object sender, EventArgs e)
        {
            Size size = mapSplitter.Panel1.ClientSize;
            tilesMapControl.ClientSize = new Size(size.Width, size.Height);
        }

        private string ResolveFileName(string filename, string path)
        {
            if (filename.IndexOf(Path.AltDirectorySeparatorChar) != -1 ||
                filename.IndexOf(Path.DirectorySeparatorChar) != -1 ||
                filename.IndexOf(Path.VolumeSeparatorChar) != -1)
                return filename;
            return Path.Combine(Path.GetDirectoryName(path), filename);
        }

        private void NoOpenError(string filename)
        {
            MessageBox.Show("Failed to open " + filename + ".");
        }

        private void LoadTilePane(string filename, Size tilesz)
        {
            if (filename == null || filename.Trim().Length == 0)
                return; // nothing..
            try
            {
                tileSize = tilesz;
                tileSet = LoadTiles(filename, tilesz);
                int width = 4;
                ITileMap map = new IdentityTileMap(
                                  new Size(width, (tileSet.GetCount() / width) + 1));
                tilesMapControl.SetMap(new SimpleTileMapView(tileSet, map));
            }
            catch (Exception)
            {
                NoOpenError(filename);
                return;
            }
        }

        private void LoadMapPane(string filename)
        {
            if (filename == null || filename.Trim().Length == 0)
                return; // nothing..
            Cursor.Current = Cursors.WaitCursor;
            try
            {
                string mapFileName  = filename;
                string tileFileName = null;
                Size   tilesz       = new Size(0, 0);
                if (filename.ToLower().EndsWith(".xml"))
                {
                    XmlDocument doc = new XmlDocument();
                    doc.Load(filename);
                    XmlElement world = XmlUtil.GetFirstElement(doc, "world"),
                               map   = XmlUtil.GetFirstElement(doc, "map"),
                               tiles = XmlUtil.GetFirstElement(doc, "tiles");
                    mapFileName  = ResolveFileName(XmlUtil.GetExpectedAttribute(map,   "file", "file"),   filename);
                    tileFileName = ResolveFileName(XmlUtil.GetExpectedAttribute(tiles, "image", "image"), filename);
                    tilesz.Width = tilesz.Height = XmlUtil.GetInt(tiles, "size");
                }
                if (tileFileName != null)
                    LoadTilePane(tileFileName, tilesz);
                tileMap = LoadMap(mapFileName);
                RefreshMap();
            }
            catch (Exception)
            {
                NoOpenError(filename);
            }
            Cursor.Current = Cursors.Default;
        }

        private void loadTiles_Click(object sender, EventArgs e)
        {
            openTilesDialog.ShowDialog(this);
            tilesField.Text = openTilesDialog.SafeFileName;
            try
            {
                int tileDim = Int32.Parse(tileSizeSelect.SelectedItem.ToString());
                if (tileDim >= 8 && tileDim <= 128)
                    tileSize = new Size(tileDim, tileDim);
            }
            catch (Exception) { }
            LoadTilePane(openTilesDialog.FileName, tileSize);
        }

        private void loadMap_Click(object sender, EventArgs e)
        {
            openMapDialog.ShowDialog(this);
            mapField.Text = openMapDialog.SafeFileName;
            LoadMapPane(openMapDialog.FileName);
        }

        private ITileSet LoadTiles(string file, Size size)
        {
            return new PageTileSet(file, size);
        }

        private ITileMap LoadMap(string file)
        {
            return TileMaps.ReadMap(file);
        }

        private void RefreshMap()
        {
            if (tileSet == null || tileMap == null)
            {
                // error dialog here
            }
            else
            {
                worldMapControl.SetMap(new SimpleTileMapView(tileSet, tileMap));
            }
        }

        private void load_Click(object sender, EventArgs e)
        {
            RefreshMap();
        }

        private Size     tileSize;
        private ITileSet tileSet = null;
        private ITileMap tileMap = null;
        private Random   random  = new Random();

        private void newMap_Click(object sender, EventArgs e)
        {
            tileMap = TileMaps.CreateMap(new Size(32, 32));
            RefreshMap();
        }

        private void saveMap_Click(object sender, EventArgs e)
        {
            if (tileMap != null)
            {
                saveMapDialog.ShowDialog(this);
                mapField.Text = saveMapDialog.FileName;
                if (mapField.Text.Trim().Length != 0)
                    tileMap.Write(saveMapDialog.FileName);
            }
        }

        private int     selected = 0;
        private Point[] marked   = new Point [] { new Point(-1, -1), new Point(-1, -1) };

        private void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            if (tileMap != null)
            {
                Point p = worldMapControl.MouseAt;
                selected = tileMap.GetTileId(p.X, p.Y);
            }
        }

        private void useTileMenuItem_Click(object sender, EventArgs e)
        {
            worldMapControl.SetFillTile(selected);
        }

        private void selectArea_Click(object sender, EventArgs e)
        {
            Cursor.Current = Cursors.Cross;
            marked[0] = worldMapControl.MouseAt;
        }

        private void mazeStyle0_Click(object sender, EventArgs e)
        {
            tileMap = TileMaps.CreateMap(new Size(16, 16));
            MazeGenerator mg = new MazeGenerator();
            mg.Create(tileMap, 100);
            RefreshMap();
        }


        private void mazeStyle1_Click(object sender, EventArgs e)
        {
            tileMap = TileMaps.CreateMap(new Size(16, 16));
            MazeGenerator mg = new MazeGenerator();
            mg.CreateFull(tileMap, 100);
            RefreshMap();
        }

        private void mazeStyle2_Click(object sender, EventArgs e)
        {
            MazeGenerator mg = new MazeGenerator();
            tileMap = mg.CreateFull1(new Size(16, 16), 100);
            RefreshMap();
        }

        private void mazeStyle3_Click(object sender, EventArgs e)
        {
            MazeGenerator mg = new MazeGenerator();
            tileMap = mg.CreateFull5(new Size(64, 64), random.Next());
            RefreshMap();
        }

        private void Translate(string filename)
        {


        }
    }
}
