using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Markup;
using Microsoft.UI.Xaml.Media;

namespace ConfigApp.Infrastructure
{
    internal static class TreeViewBuilder
    {
        private const int ConfigButtonWidth = 32;
        private const int PanelMinHeight = 24;
        private const int PanelSpacing = 8;
        
        private static DataTemplate? s_NodeTemplate;

        public static void Populate(TreeView treeView, IEnumerable<TreeMenuItem> rootItems, Action refresh)
        {
            treeView.ItemTemplate ??= GetNodeTemplate();
            treeView.RootNodes.Clear();

            foreach (var rootItem in rootItems)
                treeView.RootNodes.Add(BuildNode(rootItem, refresh));
        }

        private static DataTemplate GetNodeTemplate()
        {
            return s_NodeTemplate ??= (DataTemplate)XamlReader.Load("""
                <DataTemplate xmlns='http://schemas.microsoft.com/winfx/2006/xaml/presentation'>
                    <ContentPresenter Content='{Binding Content}' />
                </DataTemplate>
                """);
        }

        private static TreeViewNode BuildNode(TreeMenuItem item, Action refresh)
        {
            var node = new TreeViewNode
            {
                Content = BuildNodeContent(item, refresh),
                IsExpanded = true
            };

            foreach (var child in item.Children)
                node.Children.Add(BuildNode(child, refresh));

            return node;
        }

        private static StackPanel BuildNodeContent(TreeMenuItem item, Action refresh)
        {
            var panel = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = PanelSpacing,
                MinHeight = PanelMinHeight,
                Background = GetBackgroundBrush(item.IsColored)
            };

            if (item.CheckBoxVisibility == Visibility.Visible)
                panel.Children.Add(CreateCheckBox(item, refresh));

            panel.Children.Add(new TextBlock
            {
                Text = item.Text,
                VerticalAlignment = VerticalAlignment.Center,
                TextWrapping = TextWrapping.Wrap
            });

            if (item.ConfigButtonVisibility == Visibility.Visible)
                panel.Children.Add(CreateConfigButton(item, refresh));

            return panel;
        }

        private static SolidColorBrush GetBackgroundBrush(bool isColored)
        {
            return isColored
                ? new SolidColorBrush(ColorHelper.FromArgb(0xFF, 0xF6, 0xF6, 0x53))
                : new SolidColorBrush(Colors.Transparent);
        }

        private static CheckBox CreateCheckBox(TreeMenuItem item, Action refresh)
        {
            var checkBox = new CheckBox
            {
                IsChecked = item.IsChecked,
                VerticalAlignment = VerticalAlignment.Center
            };

            checkBox.Checked += (_, _) => HandleCheckStateChanged(item, true, refresh);
            checkBox.Unchecked += (_, _) => HandleCheckStateChanged(item, false, refresh);
            return checkBox;
        }

        private static void HandleCheckStateChanged(TreeMenuItem item, bool isChecked, Action refresh)
        {
            item.IsChecked = isChecked;
            item.OnCheckedClick?.Invoke();
            refresh();
        }

        private static Button CreateConfigButton(TreeMenuItem item, Action refresh)
        {
            var button = new Button
            {
                Content = "...",
                Width = ConfigButtonWidth,
                IsEnabled = item.IsConfigEnabled,
                VerticalAlignment = VerticalAlignment.Center
            };

            button.Click += async (_, _) =>
            {
                if (item.OnConfigureClickAsync is null)
                    return;

                await item.OnConfigureClickAsync();
                refresh();
            };

            return button;
        }
    }
}
