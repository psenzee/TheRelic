using System;
using System.Drawing;

namespace TileMapEd
{
    public class ImageManipulator
    {
        public static Image Cut(Image image, Point at, Size size)
        {
            Bitmap bmp = new Bitmap(size.Width, size.Height);
            Graphics g = Graphics.FromImage(bmp);
            g.DrawImage(image,
                        new Rectangle(new Point(0, 0), size),
                        new Rectangle(at, size), GraphicsUnit.Pixel);
            return bmp;
        }
        public static Image[] Untile(Image image, Size size)
        {
            int h = image.Height,
                w = image.Width;
            int count = 0;
            Image[] images = new Image [(w / size.Width) * (h / size.Height + 1)];
            for (int y = 0; y < h; y += size.Height)
                for (int x = 0; x < w; x += size.Width)
                    images[count++] = Cut(image, new Point(x, y), size);
            return images;
        }
    }
}
