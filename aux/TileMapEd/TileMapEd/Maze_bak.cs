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
        private enum WallMask { Above = 1, Below = 2, Left = 4, Right = 8, Queued = 16, Inside = 32 };
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
                    /*
                    if (maze.GetTileId(xL, yD) == 0 &&
                        maze.GetTileId(xL - 1, yD) == 1 &&
                        maze.GetTileId(xL, yD + 1) == 1)
                        maze.SetTileId(xL, yD, 1);
                    if (maze.GetTileId(xR, yU) == 0 &&
                        maze.GetTileId(xR + 1, yU) == 1 &&
                        maze.GetTileId(xR, yU - 1) == 1)
                        maze.SetTileId(xR, yU, 1);
                    if (maze.GetTileId(xR, yD) == 0 &&
                        maze.GetTileId(xR + 1, yD) == 1 &&
                        maze.GetTileId(xR, yD + 1) == 1)
                        maze.SetTileId(xR, yD, 1);
                     */
                }
            }
            return maze;
        }
        public ITileMap CreateFull5(Size size, int seed)
        {
            ITileMap map = CreateFull4(size, seed);
            return PostProcess3x3(map, GetLookup3x3Array());
        }
        /*
        public ITileMap CreateFull6(Size size, int seed)
        {
            ITileMap map = CreateFull4(size, seed);
            return PostProcess4x4(map, GetLookup4x4Array());
        }
         * */
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
        /*
        private static int[] GetLookup3x3Array()
        {
            const int ArrayCount = 512; // 2^9 because 9 = 3x3 bit pattern
            int[] lookup = new int [ArrayCount];
            for (int i = 0; i < ArrayCount; i++)
                lookup[i] = 0; // set default
            /*
            000 000 000 Floor        // 0
            111 000 000 A            // 0x1c0
            000 000 111 B            // 0x007 // not used
            100 100 100 L            // 0x124 
            001 001 001 R            // 0x049 // not used
            111 100 100 AL - Tee     // 0x1e4
            100 000 000 AL - Corner  // 0x100
             * /
            lookup[0x1c0] = 16;
            lookup[0x124] = 17;
            lookup[0x1e4] = 24;
            lookup[0x100] = 21;
            return lookup;
        }
    */

        private static int Lookup3x3(int bits)
        {
            /*
            000 000 000 Floor        // 0
            111 000 000 A            // 0x1c0
            000 000 111 B            // 0x007 // not used
            100 100 100 L            // 0x124 
            001 001 001 R            // 0x049 // not used
            111 100 100 AL - Tee     // 0x1e4
            100 000 000 AL - Corner  // 0x100
             */
            switch (bits)
            {
            case 0x1c0: return 16;
            case 0x124: return 17;
            case 0x1e4: return 24;
            case 0x100: return 21;
            }
            return 0;
        }
        private static int Lookup5x5(int bits)
        {
            /*
            000 000 000 Floor        // 0
            111 000 000 A            // 0x1c0
            000 000 111 B            // 0x007 // not used
            100 100 100 L            // 0x124 
            001 001 001 R            // 0x049 // not used
            111 100 100 AL - Tee     // 0x1e4
            100 000 000 AL - Corner  // 0x100
             */
            switch (bits)
            {
            case 0x1c0: return 16;
            case 0x124: return 17;
            case 0x1e4: return 24;
            case 0x100: return 21;
            }
            return 0;
        }
        private static ITileMap PostProcess3x3(ITileMap map)
        {
            Size originalSize = map.GetSizeInTiles(),
                 size         = new Size ((originalSize.Width  + 2) / 3, 
                                          (originalSize.Height + 2) / 3);
            ITileMap newMap = new BasicTileMap(size);
            for (int y = 0; y < size.Height; y++)
                for (int x = 0; x < size.Width; x++)
                    newMap.SetTileId(x, y, Lookup3x3(GetBitPattern(map, 3, x * 3, y * 3)));
            return newMap;
        }
        private static ITileMap PostProcess5x5(ITileMap map)
        {
            Size originalSize = map.GetSizeInTiles(),
                 size = new Size((originalSize.Width  + 2) / 3,
                                 (originalSize.Height + 2) / 3);
            ITileMap newMap = new BasicTileMap(size);
            for (int y = 0; y < size.Height; y++)
                for (int x = 0; x < size.Width; x++)
                    newMap.SetTileId(x, y, Lookup5x5(GetBitPattern(map, 5, x * 3 - 1, y * 3 - 1)));
            return newMap;
        }
        private static int AndTileId(ITileMap map, int x, int y, int value)
        {
            int v = map.GetTileId(x, y) & value;
            map.SetTileId(x, y, v);
            return v;
        }
        /*
        public void CreateFull(ITileMap maze, int offset, int seed)
        {           
            Size     size    = maze.GetSizeInTiles(),
                     qsz     = new Size(size.Width / 2, size.Height / 2);
            ITileMap quarter = new BasicTileMap(qsz);
            Create(quarter, seed);
            for (int x = 0; x < qsz.Width; ++x)
                for (int y = 0; y < qsz.Height; ++y)
                    SetQuadTile(maze, x, y, GetQuadTile(quarter.GetTileId(x,y)), offset);
            PostProcess(maze);
        }
        private static void PostProcess(ITileMap maze)
        {
            Size sz = maze.GetSizeInTiles();
            for (int x = 0; x < sz.Width; ++x)
                for (int y = 0; y < sz.Height; ++y)
                    PostProcessTile(maze, x, y);
        }
        private static bool KernelMatch(int[] kernel, int[] compare)
        {
            for (int i = 0; i < kernel.Length; i++)
                if (compare[i] != -1 && compare[i] != kernel[i])
                    return false;
            return true;
        }
        private static void Replace(ITileMap maze, int x, int y, int[] replace)
        {
            for (int j = 0; j < 3; j++)
            {
                for (int i = 0; i < 3; i++)
                {
                    int r = replace[j * 3 + i];
                    if (r != -1) maze.SetTileId(x + i - 1, y + j - 1, r);
                }
            }
        }
        private static void PostProcessTile(ITileMap maze, int x, int y)
        {
            int[] kernel = new int[9];
            for (int j = 0; j < 3; j++)
                for (int i = 0; i < 3; i++)
                    kernel[j * 3 + i] = maze.GetTileId(x + i - 1, y + j - 1);
            int[] match0         = new int[] { -1, 0, 0,
                                                      11, 5, 0,
                                                     -1, 0, 0 },
                  match0_replace = new int[] { -1, -1, -1,
                                                     -1, 19, -1,
                                                     -1, -1, -1 };
                                 
            if (KernelMatch(kernel, match0))
                Replace(maze, x, y, match0_replace);
        }
        private static void SetQuadTile(ITileMap map, int x, int y, int[] quad, int offset)
        {
            x *= 2; y *= 2;
            map.SetTileId(x + 0, y + 0, quad[0] * 32 + offset);
            map.SetTileId(x + 1, y + 0, quad[1] * 32 + offset);
            map.SetTileId(x + 0, y + 1, quad[2] * 32 + offset);
            map.SetTileId(x + 1, y + 1, quad[3] * 32 + offset);
        }
        private static int[] GetQuadTile(int v)
        {
            /*
            switch (v & 0xf)
            {
            case  1: return new int[] {  5,  5,  0,  0 };
            case  2: return new int[] {  0,  0,  4,  4 };
            case  3: return new int[] {  5,  5,  4,  4 };

            case  4: return new int[] {  6,  0,  6,  0 };
            case  5: return new int[] { 11,  5,  6,  0 };
            case  6: return new int[] {  6,  0,  8,  4 };
            case  7: return new int[] { 11,  5,  8,  4 };

            case  8: return new int[] {  0,  7,  0,  7 };
            case  9: return new int[] {  5, 10,  0,  7 };
            case 10: return new int[] {  0,  7,  4,  9 };
            case 11: return new int[] {  5, 10,  4,  9 };

            case 12: return new int[] {  6,  7,  6,  7 };
            case 13: return new int[] { 11, 10,  6,  7 };
            case 14: return new int[] {  6,  7,  8,  9 };
            case 15: return new int[] { 11, 10,  8,  9 };
            }
             * /
            switch (v & 0xf)
            {
            case  0: return new int[] {  0,  0,  0,  0 };
            case  1: return new int[] {  5,  5,  0,  0 };
            case  2: return new int[] {  0,  0,  0,  0 };
            case  3: return new int[] {  5,  5,  0,  0 };

            case  4: return new int[] {  6,  0,  6,  0 };
            case  5: return new int[] { 11,  5,  6,  0 };
            case  6: return new int[] {  6,  0,  6,  0 };
            case  7: return new int[] { 11,  5,  6,  0 };

            case  8: return new int[] {  0,  0,  0,  0 };
            case  9: return new int[] {  5,  5,  0,  0 };
            case 10: return new int[] {  0,  0,  0,  0 };
            case 11: return new int[] {  5,  5,  0,  0 };

            case 12: return new int[] {  6,  0,  6,  0 };
            case 13: return new int[] { 11,  5,  6,  0 };
            case 14: return new int[] {  6,  0,  6,  0 };
            case 15: return new int[] { 11,  5,  6,  0 };
            }
            return new int[] { 0, 0, 0, 0 };
        }*/
    }
}