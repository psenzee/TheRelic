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
    public class MazeGenerator
    {
        public enum WallType { None               = 0,
                               Corner             = 21,
                               Vertical           = 17,
                               VerticalEndTop     = 18,
                               VerticalEndBottom  = 23,
                               Horizontal         = 16,
                               HorizontalEndLeft  = 19,
                               HorizontalEndRight = 20,
                               Tee                = 24 };

        //private enum WallMask { Above = 1, Below = 2, Left = 4, Right = 8, Queued = 16, Inside = 32 };
        /*
         * For the benefit of novice programmers, I've listed
         * the various bitmasks used to the best of my ability
         * (I don't use Java, I'm assuming it uses standard coordinates where 0,0 is the top left)
         * I've commented on the drawing code as well
         *
         *  1: Wall above
         *  2: Wall below
         *  4: Wall left
         *  8: Wall right
         * 16: queued to be added
         * 32: "in" the maze
         *
         *  - quin/10-24-06
         */

        public void Create(ITileMap maze, int seed)
        {
            Random random = new Random(seed);
            Size   size   = maze.GetSizeInTiles();

            int x = 0, y = 0, n = 0, d = 0;

            int[] dx = new int [] { 0, 0, -1, 1 };
            int[] dy = new int [] { -1, 1, 0, 0 };
            int[] todo = new int [size.Width * size.Height];
            int   todonum = 0;
      
            // We start with a grid full of walls. 
            for (x = 0; x < size.Width; ++x)
            {
                for (y = 0; y < size.Height; ++y)
                {
                    if (x == 0 || x == (size.Width - 1) || y == 0 || y == (size.Height - 1))
                        maze.SetTileId(x, y, 32);
                    else
                        maze.SetTileId(x, y, 63);
                }
            }
      
            // Select any square of the grid, to start with. 
            x = (int) (1 + random.NextDouble() * (size.Width  - 2));
            y = (int) (1 + random.NextDouble() * (size.Height - 2));
      
            // Mark this square as connected to the maze. 
            AndTileId(maze, x, y, ~48);
      
            // Remember the surrounding squares, as we will 
            for (d = 0; d < 4; ++d)
            {
                if ((maze.GetTileId(x + dx[d], y + dy[d]) & 16) != 0)
                {
                    // want to connect them to the maze. 

                    /* alternately, you could use a struct to store the two integers
                     * this would result in easier to read code, though not as speedy
                     * of course, if you were worried about speed, you wouldn't be using Java
                     * you could also use a single integer which represents (x + y * width)
                     * this would actually be faster than the current approach
                     * - quin/10-24-06
                     *    Actually, the former wouldn't work in Java- there's no such thing as a
                     *    struct. It's a class or nothing, I'm afraid.
                     *    - Jae Armstrong/23-03-07
                     */                    
                    todo[todonum++] = ((x + dx[d]) << 16) | (y + dy[d]);
                    AndTileId(maze, x + dx[d], y + dy[d], ~16);
                }
            }
      
            // We won't be finished until all is connected. 
            while (todonum > 0)
            {
                // We select one of the squares next to the maze. 
                n = (int) (random.NextDouble() * todonum);
                x = todo[n] >> 16; // the top 2 bytes of the data 
                y = todo[n] & 65535; // the bottom 2 bytes of the data 
      
                // We will connect it, so remove it from the queue. 
                todo[n] = todo[--todonum];
      
                // Select a direction, which leads to the maze. 
                do
                {
                   d = (int) (random.NextDouble() * 4);
                }
                while ((maze.GetTileId(x + dx[d], y + dy[d]) & 32) != 0);
      
                // Connect this square to the maze. 
                AndTileId(maze, x,         y,         ~((1 << d) | 32));
                AndTileId(maze, x + dx[d], y + dy[d], ~( 1 << (d ^ 1)));
      
                // Remember the surrounding squares, which aren't 
                for (d = 0; d < 4; ++d)
                {
                    if ((maze.GetTileId(x + dx[d], y + dy[d]) & 16) != 0)
                    {                       
                        // connected to the maze, and aren't yet queued to be. 
                        todo[todonum++] = ((x + dx[d]) << 16) | (y + dy[d]);
                        AndTileId(maze, x + dx[d], y + dy[d], ~16);
                    }
                }
                // Repeat until finished.
            }

            // One may want to add an entrance and exit. 
            AndTileId(maze, 1, 1, ~1);                            // we'll want to put these in different spots..
            AndTileId(maze, size.Width - 2, size.Height - 2, ~2);

            // Clean up everything..
            for (x = 0; x < size.Width; ++x)
                for (y = 0; y < size.Height; ++y)
                    AndTileId(maze, x, y, 15);
        }
        public void CreateFull(ITileMap maze, int seed)
        {
            Size size = maze.GetSizeInTiles(),
                 qsz  = new Size((size.Width - 1) / 2, (size.Height - 1) / 2);
            ITileMap quarter = new BasicTileMap(qsz);
            Create(quarter, seed);
            for (int x = 0; x < size.Width; ++x)
                for (int y = 0; y < size.Height; ++y)
                    maze.SetTileId(x, y, 0);
            int dx = 1, dy = 1;
            for (int x = 0; x < qsz.Width; ++x)
            {
                dx = 1;
                for (int y = 0; y < qsz.Height; ++y)
                {
                    int id = quarter.GetTileId(x, y);
                    int mask = ((id & 2) > 0 ? 0x1c0 : 0) |
                               ((id & 1) > 0 ? 0x007 : 0) |
                               ((id & 8) > 0 ? 0x124 : 0) |
                               ((id & 4) > 0 ? 0x049 : 0);
                    for (int k = 0; k < 9; k++)
                    {
                        int mx = dx + (k % 3) - 1, my = dy + (k / 3) - 1;
                        if ((k % 3) > 0 && (k / 3) > 0)
                        maze.SetTileId(mx, my, maze.GetTileId(mx, my) | ((mask >> k) & 1));
                    }
                    dx += 2;
                }
                dy += 2;
            }
        }
        public ITileMap CreateFull1(Size size, int seed)
        {
            Size fsz = new Size(size.Width * 2 + 2, size.Height * 2 + 2);
            ITileMap small = new BasicTileMap(size),
                     maze  = new BasicTileMap(fsz);
            Create(small, seed);
            for (int x = 0; x < fsz.Width; ++x)
                for (int y = 0; y < fsz.Height; ++y)
                    maze.SetTileId(x, y, 0);
            int dx = 1, dy = 1;
            for (int x = 0; x < size.Width; ++x)
            {
                dx = 1;
                for (int y = 0; y < size.Height; ++y)
                {
                    int id = small.GetTileId(x, y);
                    int mask = ((id & 2) > 0 ? 0x1c0 : 0) |
                               ((id & 1) > 0 ? 0x007 : 0) |
                               ((id & 8) > 0 ? 0x124 : 0) |
                               ((id & 4) > 0 ? 0x049 : 0);
                    for (int k = 0; k < 9; k++)
                    {
                        int mx = dx + (k % 3) - 1, my = dy + (k / 3) - 1;
                        if ((k % 3) > 0 && (k / 3) > 0)
                            maze.SetTileId(mx, my, maze.GetTileId(mx, my) | ((mask >> k) & 1));
                    }
                    dx += 2;
                }
                dy += 2;
            }
            return maze;
        }
        public ITileMap CreateFull2(Size size, int seed)
        {
            Size     fsz   = new Size(size.Width * 3, size.Height * 3);
            ITileMap small = new BasicTileMap(size),
                     maze = new BasicTileMap(fsz);
            Create(small, seed);
            for (int x = 0; x < fsz.Width; ++x)
                for (int y = 0; y < fsz.Height; ++y)
                    maze.SetTileId(x, y, 0);
            for (int x = 0; x < size.Width; ++x)
            {
                for (int y = 0; y < size.Height; ++y)
                {
                    int id   = small.GetTileId(x, y),
                        mask = ((id & 2) > 0 ? 0x1c0 : 0) |
                               ((id & 1) > 0 ? 0x007 : 0) |
                               ((id & 8) > 0 ? 0x124 : 0) |
                               ((id & 4) > 0 ? 0x049 : 0);
                    for (int k = 0; k < 9; k++)
                        maze.SetTileId((k % 3) + x * 3, (k / 3) + y * 3, 
                            ((mask >> k) & 1));
                }
            }
            return maze;
        }
        public ITileMap CreateFull3(Size size, int seed)
        {
            Size     fsz   = new Size(size.Width * 4, size.Height * 4);
            ITileMap small = new BasicTileMap(size),
                     maze  = new BasicTileMap(fsz);
            Create(small, seed);
            for (int x = 0; x < fsz.Width; ++x)
                for (int y = 0; y < fsz.Height; ++y)
                    maze.SetTileId(x, y, 0);
            for (int x = 0; x < size.Width; ++x)
            {
                for (int y = 0; y < size.Height; ++y)
                {
                    int id   = small.GetTileId(x, y),
                        mask = ((id & 2) > 0 ? 0xf000 : 0) |
                               ((id & 1) > 0 ? 0x000f : 0) |
                               ((id & 8) > 0 ? 0x8888 : 0) |
                               ((id & 4) > 0 ? 0x1111 : 0);
                    for (int k = 0; k < 16; k++)
                        maze.SetTileId((k % 4) + x * 4, (k / 4) + y * 4,
                            ((mask >> k) & 1));
                }
            }
            return maze;
        }
        public ITileMap CreateFull4(Size size, int seed)
        {
            const int Multiple = 6;
            Size     fsz   = new Size(size.Width * Multiple, size.Height * Multiple);
            ITileMap small = new BasicTileMap(size),
                     maze  = new BasicTileMap(fsz);
            Create(small, seed);
            for (int y = 0; y < size.Height; y++)
                if ((small.GetTileId(size.Width - 2, y) & 8) > 0)
                    small.SetTileId(size.Width - 1, y, 4);
            for (int x = 0; x < size.Width; x++)
                if ((small.GetTileId(x, size.Height - 2) & 2) > 0)
                    small.SetTileId(x, size.Height - 1, 1);
            for (int x = 0; x < fsz.Width; ++x)
                for (int y = 0; y < fsz.Height; ++y)
                    maze.SetTileId(x, y, 0);
            for (int x = 0; x < size.Width; ++x)
            {
                for (int y = 0; y < size.Height; ++y)
                {
                    int  id   = small.GetTileId(x, y);
                    long mask = ((id & 2) > 0 ? 0L/*0xfc0000000L*/ : 0) |
                                ((id & 1) > 0 ? 0x00000003fL : 0) |
                                ((id & 8) > 0 ? 0L/*0x820820820L*/ : 0) |
                                ((id & 4) > 0 ? 0x041041041L : 0);
                    for (int k = 0; k < Multiple * Multiple; k++)
                        maze.SetTileId((k % Multiple) + x * Multiple, 
                                       (k / Multiple) + y * Multiple,
                                       (int)((mask >> k) & 1));
                }
            }
            for (int x = 0; x < fsz.Width; x += Multiple)
            {
                for (int y = 0; y < fsz.Height; y += Multiple)
                {
                    int xL = x,                yU = y,
                        xR = x + Multiple - 1, yD = y + Multiple - 1;
                    if (maze.GetTileId(xL, yU) == 0 &&
                        maze.GetTileId(xL - 1, yU) == 1 &&
                        maze.GetTileId(xL, yU - 1) == 1)
                        maze.SetTileId(xL, yU, 1);
                }
            }
            return maze;
        }
        public ITileMap CreateFull5(Size size, int seed)
        {
            ITileMap map = CreateFull4(size, seed);
            map = PostProcess5x5(map);
            Size sz = map.GetSizeInTiles();
            return TileMaps.Copy(map, new Rectangle(2, 2, sz.Width - 3, sz.Height - 3));
        }
        private static int GetBitPattern(ITileMap map, int size, int x, int y)
        {
            int pattern = 0;
            for (int j = 0; j < size; j++)
            {
                for (int i = 0; i < size; i++)
                {
                    pattern <<= 1;
                    if (map.GetTileId(x + i, y + j) > 0)
                        pattern |= 1;
                }
            }
            return pattern;
        }
        private static WallType Lookup5x5(int bits)
        {
            WallType id = WallType.None;
            switch (bits & 0x00739c0)
            {
            case 0x0040000: id = WallType.Corner;     break; 
            case 0x0042100: id = WallType.Vertical;   break; 
            case 0x0070000: id = WallType.Horizontal; break; 
            case 0x0072100: id = WallType.Tee;        break; 
            }
            if (id == WallType.Vertical   && (bits & 0x0800000) == 0) id = WallType.VerticalEndTop;
            if (id == WallType.Vertical   && (bits & 0x0000008) == 0) id = WallType.VerticalEndBottom;
            if (id == WallType.Horizontal && (bits & 0x0080000) == 0) id = WallType.HorizontalEndLeft;
            if (id == WallType.Horizontal && (bits & 0x0008000) == 0) id = WallType.HorizontalEndRight;
            return id;
        }
        private static ITileMap PostProcess5x5(ITileMap map)
        {
            Size originalSize = map.GetSizeInTiles(),
                 size = new Size((originalSize.Width  + 2) / 3,
                                 (originalSize.Height + 2) / 3);
            ITileMap newMap = new BasicTileMap(size);
            for (int y = 0; y < size.Height; y++)
                for (int x = 0; x < size.Width; x++)
                    newMap.SetTileId(x, y, (int)Lookup5x5(GetBitPattern(map, 5, x * 3 - 1, y * 3 - 1)));
            return newMap;
        }
        private static int AndTileId(ITileMap map, int x, int y, int value)
        {
            int v = map.GetTileId(x, y) & value;
            map.SetTileId(x, y, v);
            return v;
        }
    }
}