using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace TileMapEd
{
    public partial class MapControl : UserControl
    {
        public MouseClickInfo MouseLeft
        {
            get { return mouseLeft; }
        }

        public MouseClickInfo MouseRight
        {
            get { return mouseRight; }
        }

        public Point          MouseAt
        {
            get { return mouseAt; }
        }

        public Point          ClickAt
        {
            get { return clickAt; }
        }

        public MapControl()
        {
            InitializeComponent();
            InitializeMapComponent();
        }

        public void SetMap(ITileMapView map)
        {
            this.map = map;
            Size mapSize = map.GetMapSizeInTiles();
            columns = new DataGridViewColumn[mapSize.Width];
            view.CellBorderStyle = DataGridViewCellBorderStyle.None;
            view.CellClick += new DataGridViewCellEventHandler(view_CellClick);
            view.CellMouseEnter += new DataGridViewCellEventHandler(view_CellMouseEnter);
            view.CellMouseDown += new DataGridViewCellMouseEventHandler(view_CellMouseDown);
            view.CellMouseUp += new DataGridViewCellMouseEventHandler(view_CellMouseUp);
            for (int i = 0; i < mapSize.Width; i++)
            {
                DataGridViewImageColumn column = new DataGridViewImageColumn();
                column.HeaderText = "";
                column.Name = "";
                columns[i] = column;
                column.DividerWidth = 0;
                column.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            }
            view.Columns.Clear();
            view.Columns.AddRange(this.columns);
            view.Rows.Clear();
            Object[] images = new Object[mapSize.Width];
            for (int r = 0; r < mapSize.Height; r++)
            {
                for (int c = 0; c < mapSize.Width; c++)
                    images[c] = map.GetRenderableObject(c, r);
                view.Rows.Add(images);
            }
        }

        public enum EditMode { Select, Fill };

        public void SetEditMode(EditMode mode)
        {
            this.editMode = mode;
        }

        public EditMode GetEditMode()
        {
            return this.editMode;
        }

        public void SetFillTile(int id)
        {
            this.fillTile = id;
            if (this.fillChanged != null)
               fillChanged(this, id);
        }

        public int GetFillTile()
        {
            return this.fillTile;
        }

        public int GetSelectedId()
        {
            return this.selectedId;
        }

        public delegate void MapControlFillChanged(MapControl sender, int fillId);

        public MapControlFillChanged fillChanged = null;

        public void SetTileMap(ITileMap map)
        {
            tileMap = map;
        }

        public ITileMap GetTileMap()
        {
            return tileMap;
        }

        public void SetMapTabControl(MapTabControl control)
        {
            tabControl = control;
        }

        // Event Handlers
        protected override void OnSizeChanged(EventArgs e)
        {
            ChangeSize(e);
        }

        public void ChangeSize(EventArgs e)
        {
            view.Size = this.ClientSize;
            base.OnSizeChanged(e);
        }

        public void view_CellClick(Object sender, DataGridViewCellEventArgs e)
        {
            mouseAt = new Point(e.ColumnIndex, e.RowIndex);
            clickAt = mouseAt;
            if (e.ColumnIndex >= 0 && e.RowIndex >= 0)
            {
                switch (editMode)
                {
                case EditMode.Fill:
                    if (map.SetTileId(e.ColumnIndex, e.RowIndex, GetFillTile()))
                        view[e.ColumnIndex, e.RowIndex].Value = map.GetRenderableObject(GetFillTile());
                    break;
                case EditMode.Select:
                    SetFillTile(map.GetTileMap().GetTileId(e.ColumnIndex, e.RowIndex));
                    break;
                }
            }
        }

        private void SetMouseClickInfo(DataGridViewCellMouseEventArgs e, MouseClickInfo.ClickState state)
        {
            Point at = new Point(e.ColumnIndex, e.RowIndex);
            if      (e.Button == MouseButtons.Left)
                mouseLeft.SetState(state, at);
            else if (e.Button == MouseButtons.Right)
                mouseRight.SetState(state, new Point(e.ColumnIndex, e.RowIndex));
        }

        private void view_CellMouseDown(Object sender, DataGridViewCellMouseEventArgs e)
        {
            SetMouseClickInfo(e, MouseClickInfo.ClickState.Down);
        }

        private void view_CellMouseUp(Object sender, DataGridViewCellMouseEventArgs e)
        {
            SetMouseClickInfo(e, MouseClickInfo.ClickState.Up);
        }

        private void view_CellMouseEnter(Object sender, DataGridViewCellEventArgs e)
        {
            mouseAt = new Point(e.ColumnIndex, e.RowIndex);
            if (editMode == EditMode.Fill && MouseLeft.State == MouseClickInfo.ClickState.Down && e.ColumnIndex >= 0 && e.RowIndex >= 0)
            {
                if (map.SetTileId(e.ColumnIndex, e.RowIndex, GetFillTile()))
                    view[e.ColumnIndex, e.RowIndex].Value = map.GetRenderableObject(GetFillTile());
            }
        }

        private void view_Scroll(Object sender, ScrollEventArgs e)
        {
            if (tabControl == null)
                return;

            if (e.ScrollOrientation == ScrollOrientation.HorizontalScroll)
            {
                tabControl.SetFirstDisplayedColumn(view.FirstDisplayedScrollingColumnIndex);
            }
            else if (e.ScrollOrientation == ScrollOrientation.VerticalScroll)
            {
                tabControl.SetFirstDisplayedRow(view.FirstDisplayedScrollingRowIndex);
            }
        }

        public void SetFirstDisplayedColumn(int index)
        {
            view.FirstDisplayedScrollingColumnIndex = index;
        }

        public void SetFirstDisplayedRow(int index)
        {
            view.FirstDisplayedScrollingRowIndex = index;
        }

        // private
        private void InitializeMapComponent()
        {
            // view
            view = new DataGridView();
            ((ISupportInitialize)(view)).BeginInit();
            SuspendLayout();
            view.CellBorderStyle = DataGridViewCellBorderStyle.None;
            view.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            view.ColumnHeadersHeight = 4;
            view.Location = new Point(0, 0);
            view.Name = "View";
            view.Size = ClientSize;
            view.TabIndex = 0;
            view.AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.AllCells;
            view.RowHeadersWidth = 4;
            view.Scroll += new ScrollEventHandler(view_Scroll);

            // MapControl
            Controls.Add(view);
            Name = Text = "MapControl";
            ((ISupportInitialize)(view)).EndInit();
            ResumeLayout(false);
        }

        private MapTabControl        tabControl     = null;

        private ITileMap             tileMap        = null;
        private ITileMapView         map            = null;
        private int                  fillTile       = 0,
                                     selectedId     = 0;
        private EditMode             editMode       = EditMode.Select;
        private DataGridView         view           = null;
        private DataGridViewColumn[] columns        = null;
        private MouseClickInfo       mouseLeft      = new MouseClickInfo(),
                                     mouseRight     = new MouseClickInfo();
        private Point                mouseAt        = new Point(-1, -1),
                                     clickAt        = new Point(-1, -1);
    }

    public class MouseClickInfo
    {
        public enum ClickState { Up, Down };

        public ClickState State
        {
            get { return state; }
        }

        public void SetState(ClickState state, Point at)
        {
            this.state = state;
            if (state == ClickState.Down)
            {
                down = at;
                up = new Point(-1, -1);
            }
            else
            {
                up = at;
            }
        }

        private ClickState state = ClickState.Up;
        private Point down = new Point(-1, -1);
        private Point up = new Point(-1, -1);
    }
}
