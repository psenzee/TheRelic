using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace TileMapEd
{
    public partial class MapEditor : Form
    {
        public MapEditor()
        {
            InitializeComponent();
        }
        
        protected override void OnSizeChanged(EventArgs e)
        {
            mapEditorControl.Size = this.ClientSize;
            base.OnSizeChanged(e);
        }
    }
}
