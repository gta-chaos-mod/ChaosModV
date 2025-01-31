using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs
{
    public class ChaosGrid
    {
        public Grid Grid { get; private set; } = new Grid();

        private int m_CurrentRow = -1;
        private int m_CurrentColumn = 0;
        private readonly List<RowDefinition> m_RowDefinitions = new();
        private readonly List<ColumnDefinition> m_ColumnDefinitions = new();

        public ChaosGrid(Grid? grid = null)
        {
            Init(grid);
        }

        public virtual void Init(Grid? grid = null)
        {
            if (grid is not null && grid == Grid)
                return;

            Grid = grid ?? new Grid();

            m_CurrentRow = -1;
            m_CurrentColumn = 0;

            m_RowDefinitions.Clear();
            m_ColumnDefinitions.Clear();

            PopRow();
        }

        public void PushNewColumn(GridLength gridLength)
        {
            var columnDefinition = new ColumnDefinition()
            {
                Width = gridLength,
            };
            m_ColumnDefinitions.Add(columnDefinition);
            Grid.ColumnDefinitions.Add(columnDefinition);
        }

        private void CheckColumnValidity()
        {
            if (m_ColumnDefinitions.Count <= m_CurrentColumn)
                throw new System.IndexOutOfRangeException("m_CurrentColumn > max columns!");
        }

        public void SetRowHeight(GridLength gridLength)
        {
            m_RowDefinitions[m_CurrentRow].Height = gridLength;
        }

        private void PushRow(string? text, UIElement? control)
        {
            if (text != null)
            {
                CheckColumnValidity();

                var textBlock = new TextBlock
                {
                    Text = text,
                    HorizontalAlignment = HorizontalAlignment.Right,
                    VerticalAlignment = control != null && control.GetValue(FrameworkElement.VerticalAlignmentProperty) != null
                        ? (VerticalAlignment)control.GetValue(FrameworkElement.VerticalAlignmentProperty) : VerticalAlignment.Center
                };

                textBlock.SetValue(Grid.ColumnProperty, m_CurrentColumn);
                textBlock.SetValue(Grid.RowProperty, m_CurrentRow);
                Grid.Children.Add(textBlock);

                m_CurrentColumn++;
            }

            if (control != null)
            {
                CheckColumnValidity();

                if (text != null)
                {
                    if (control.GetValue(FrameworkElement.HorizontalAlignmentProperty) == null)
                        control.SetValue(FrameworkElement.HorizontalAlignmentProperty, HorizontalAlignment.Right);
                    if (control.GetValue(FrameworkElement.VerticalAlignmentProperty) == null)
                        control.SetValue(FrameworkElement.VerticalAlignmentProperty, VerticalAlignment.Center);
                }

                control.SetValue(Grid.ColumnProperty, m_CurrentColumn);
                control.SetValue(Grid.RowProperty, m_CurrentRow);
                Grid.Children.Add(control);

                m_CurrentColumn++;
            }
        }

        public void PopRow()
        {
            m_CurrentColumn = 0;
            m_CurrentRow++;

            var rowDefinition = new RowDefinition()
            {
                Height = new GridLength(40f)
            };
            m_RowDefinitions.Add(rowDefinition);
            Grid.RowDefinitions.Add(rowDefinition);
        }

        public void PushRowEmpty()
        {
            CheckColumnValidity();

            m_CurrentColumn++;
        }

        public void PushRowEmptyPair()
        {
            CheckColumnValidity();

            m_CurrentColumn += 3;
        }

        public void PushRowElement(UIElement element)
        {
            PushRow(null, element);
        }

        public void PushRowExpandElement(UIElement element)
        {
            element.ClipToBounds = false;

            var canvas = new Canvas();
            canvas.Children.Add(element);
            PushRow(null, canvas);
        }

        public void PushRowPair(string text, UIElement element)
        {
            PushRow(text, element);
        }

        public void PushRowSpacedPair(string text, UIElement element)
        {
            PushRow(text, null);
            PushRowEmpty();

            element.SetValue(FrameworkElement.HorizontalAlignmentProperty, HorizontalAlignment.Left);
            element.SetValue(FrameworkElement.VerticalAlignmentProperty, VerticalAlignment.Center);

            PushRow(null, element);
        }

        public void PushRowTextBlock(string text)
        {
            PushRow(text, null);
        }
    }
}
