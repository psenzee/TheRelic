using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace TileMapEd
{
    public interface ITile
    {
        Image GetRenderableObject(); // this is a deliberately ambiguous name, we'd like to share this architecture around
    }
    
    public class ImageTile : ITile
    {
        private Image image;

        public ImageTile(Image image)
        {
            this.image = image;
        }
        public Image GetRenderableObject()
        {
            return image;
        }
    }

    public interface ITileSet
    {
        int   GetCount();
        Size  GetTileSize();
        ITile GetTile(int index);
    }

    public class PageTileSet : ITileSet
    {
        private Size    size;
        private ITile[] tiles = null;

        public PageTileSet(string filename, Size size)
        {
            this.size   = size;
            Image image = Image.FromFile(filename);
            Image[] images = ImageManipulator.Untile(image, size);
            this.tiles = new ITile [images.Length];
            for (int i = 0; i < images.Length; i++)
                this.tiles[i] = new ImageTile(images[i]);
        }
        public int GetCount()
        {
            return tiles.Length;
        }
        public Size GetTileSize()
        {
            return size;
        }        
        public ITile GetTile(int index)
        {
            return tiles[index % GetCount()];
        }
    }

    public interface ITileMap
    {
        int  GetTileId(int x, int y);
        bool SetTileId(int x, int y, int id);
        Size GetSizeInTiles();

        void Write(string filename);
    }

    public class TileMaps
    {
        public static ITileMap CreateMap(Size size)
        {
            return new BasicTileMap(size, 0);
        }
        public static void Copy(ITileMap src, ITileMap dst)
        {
            Copy(src, new Rectangle(new Point(0, 0), src.GetSizeInTiles()),
                 dst, new Rectangle(new Point(0, 0), dst.GetSizeInTiles()));
        }
        public static ITileMap ReadMap(string filename)
        {
            ITileMap map = null;
            filename = filename.ToLower();
            if      (filename.EndsWith(".emp"))
                map = new BasicTileMap(filename);
            else if (filename.EndsWith(".map"))
            {
                ITileMap umap = new UndeadTileMap(filename);
                map = new BasicTileMap(umap.GetSizeInTiles());
                Copy(umap, map); // let's not hang on to undead map types..
            }
            else
            {
                // error..
            }
            return map;
        }
        public static void Copy(ITileMap src, Rectangle from, ITileMap dst, Rectangle to)
        {
            if (from.X < 0) from.X = 0;
            if (from.Y < 0) from.Y = 0;
            if (to.X   < 0) to.X   = 0;
            if (to.Y   < 0) to.Y   = 0;
            Size ssz   = src.GetSizeInTiles(),
                 dsz   = dst.GetSizeInTiles(),
                 sz    = new Size(Math.Min(Math.Min(ssz.Width  - from.X, from.Width),
                                           Math.Min(dsz.Width  - to.X,   to.Width)),
                                  Math.Min(Math.Min(ssz.Height - from.Y, from.Height),
                                           Math.Min(dsz.Height - to.Y,   to.Height)));
            CopyRaw(src, new Point(from.X, from.Y), dst, new Point(to.X, to.Y), sz);
        }
        public static ITileMap Copy(ITileMap src, Rectangle from)
        {
            ITileMap map = new BasicTileMap(from.Size);
            Copy(src, from, map, new Rectangle(new Point(0, 0), from.Size));
            return map;
        }
        private static void CopyRaw(ITileMap src, Point from, ITileMap dst, Point to, Size sz)
        {
            for (int y = 0; y < sz.Height; y++)
                for (int x = 0; x < sz.Width; x++)
                    dst.SetTileId(to.X + x, to.Y + y, src.GetTileId(from.X + x, from.Y + y));
        }
    }

    public class UndeadTileMap : ITileMap
    {
        Size   size;
        byte[] data   = null;

        public UndeadTileMap(string filename)
        {
            FileStream fs = null;
            try
            {
                fs = File.OpenRead(filename);
                BinaryReader reader = new BinaryReader(fs);
                size.Width  = reader.ReadByte();
                size.Height = reader.ReadByte();
                int area = size.Width * size.Height;
                data = new byte [area];
                reader.Read(data, 0, area);
            }
            finally
            {
                if (fs != null)
                    fs.Close();
            }
        }
        public int GetTileId(int x, int y)
        {
            if (x >= size.Width || y >= size.Height)
                return 0;
            return data[y * size.Width + x];
        }
        public bool SetTileId(int x, int y, int id)
        {
            if (x >= size.Width || y >= size.Height || id < 0 || id >= 256)
                return false;
            data[y * size.Width + x] = (byte)id;
            return true;
        }
        public Size GetSizeInTiles()
        {
            return size;
        }
        public void Write(string filename)
        {
            throw new Exception("format does not currently support saving.");
        }
    }

    public class BasicTileMap : ITileMap
    {
        public BasicTileMap(Size size)
        {
            Create(size, 0);
        }

        public BasicTileMap(Size size, int initialValue)
        {
            Create(size, initialValue);
        }

        public BasicTileMap(string filename)
        {
            FileStream fs = null;
            try
            {
                fs = File.OpenRead(filename);
                BinaryReader reader = new BinaryReader(fs);
                /*
                byte[] magic = new byte[] { (byte)'T', (byte)'I', (byte)'L', (byte)'0' },
                       signature = new byte[4];
                reader.Read(signature, 0, 4);
                if (!Equals(magic, signature))
                    throw new Exception("bad file format!");
                 */
                size.Width  = reader.ReadInt32();
                size.Height = reader.ReadInt32();
                if (size.Width < 0 || size.Height < 0)
                    throw new Exception("bad file format!");
                Create(size, 0);
                ReadIntArray(reader, data, 0, size.Width * size.Height);
            }
            finally
            {
                if (fs != null)
                    fs.Close();
            }
        }

        public int GetTileId(int x, int y)
        {
            if (x < 0 || y < 0 || x >= size.Width || y >= size.Height)
                return 0;
            return data[y * size.Width + x];
        }

        public bool SetTileId(int x, int y, int id)
        {
            if (x >= size.Width || y >= size.Height || id < 0)
                return false;
            data[y * size.Width + x] = id;
            return true;
        }

        public Size GetSizeInTiles()
        {
            return size;
        }

        public void Write(string filename)
        {
            FileStream fs = null;
            try
            {
                fs = File.OpenWrite(filename);
                BinaryWriter writer = new BinaryWriter(fs);
                //byte[] magic = new byte[] { (byte)'T', (byte)'I', (byte)'L', (byte)'0' };
                //writer.Write(magic, 0, 4);
                writer.Write(size.Width);
                writer.Write(size.Height);
                WriteIntArray(writer, data, 0, size.Width * size.Height);
            }
            finally
            {
                if (fs != null)
                    fs.Close();
            }
        }

        // private
        private Size  size;
        private int[] data = null;

        private static bool Equals(byte[] a, byte[] b)
        {
            if (a == b)
                return true;
            else if (a == null || b == null)
                return false;
            else if (a.Length != b.Length)
                return false;
            for (int i = 0; i < a.Length; i++)
                if (a[i] != b[i])
                    return false;
            return true;
        }

        private void Create(Size size, int value)
        {
            this.size = size;
            int area = size.Width * size.Height;
            data = new int[area];
            for (int i = 0; i < area; i++)
                data[i] = value;
        }

        private static void ReadIntArray(BinaryReader file, int[] data, int offset, int count)
        {
            for (int i = 0; i < count; i++)
                data[i + offset] = file.ReadInt32();
        }

        private static void WriteIntArray(BinaryWriter file, int[] data, int offset, int count)
        {
            for (int i = 0; i < count; i++)
                file.Write(data[i + offset]);
        }
    }

    public class IdentityTileMap : ITileMap
    {
        public IdentityTileMap(Size size)
        {
            this.size = size;
        }
        public int GetTileId(int x, int y)
        {
            if (x >= size.Width || y >= size.Height)
                return 0;
            return y * size.Width + x;
        }
        public bool SetTileId(int x, int y, int id)
        {
            return false;
        }
        public Size GetSizeInTiles()
        {
            return size;
        }
        public void Write(string filename)
        {
            throw new Exception("format does not currently support saving.");
        }

        private Size size;
    }

    public interface ITileMapView
    {
        ITileSet GetTileSet();
        ITileMap GetTileMap();

        ITile    GetTile(int x, int y);
        ITile    GetTile(int index);

        int      GetTileId(int x, int y);
        bool     SetTileId(int x, int y, int index);

        Size     GetTileSize();
        Size     GetMapSizeInTiles();
        Size     GetMapSizeInPixels();

        Image    GetRenderableObject(int x, int y);
        Image    GetRenderableObject(int index);
    }

    public class SimpleTileMapView : ITileMapView
    {
        public SimpleTileMapView(ITileSet set, ITileMap map)
        {
            this.set = set;
            this.map = map;
        }

        public ITileSet GetTileSet()
        {
            return set;
        }
        public ITileMap GetTileMap()
        {
            return map;
        }
        public ITile GetTile(int x, int y)
        {
            return set.GetTile(map.GetTileId(x, y));
        }
        public ITile GetTile(int index)
        {
            return set.GetTile(index);
        }
        public int GetTileId(int x, int y)
        {
            return map.GetTileId(x, y);
        }
        public bool SetTileId(int x, int y, int index)
        {
            if (index >= set.GetCount())
                return false;
            return map.SetTileId(x, y, index);
        }
        public Size GetTileSize()
        {
            return set.GetTileSize();
        }
        public Size GetMapSizeInTiles()
        {
            return map.GetSizeInTiles();
        }
        public Size GetMapSizeInPixels()
        {
            Size mapsz  = GetMapSizeInTiles(),
                 tilesz = GetTileSize();
            return new Size(mapsz.Width * tilesz.Width, mapsz.Height * tilesz.Height);
        }

        public Image GetRenderableObject(int x, int y)
        {
            return GetTile(x, y).GetRenderableObject();
        }
        public Image GetRenderableObject(int index)
        {
            return GetTile(index).GetRenderableObject();
        }

        private ITileSet set = null;
        private ITileMap map = null;
    }
}