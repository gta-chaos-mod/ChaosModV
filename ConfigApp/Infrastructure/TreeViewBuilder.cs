using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;

namespace ConfigApp.Infrastructure
{
    public static class TreeViewBuilder
    {
        public static void Populate(TreeView treeView, IEnumerable<TreeMenuItem> rootItems, Action refresh)
        {
            treeView.RootNodes.Clear();

            foreach (var rootItem in rootItems)
                treeView.RootNodes.Add(BuildNode(rootItem, refresh));
        }

        private static TreeViewNode BuildNode(TreeMenuItem item, Action refresh)
        {
            var panel = new StackPanel
            {
                Orientation = Orientation.Horizontal,
                Spacing = 8,
                MinHeight = 24,
                Background = new SolidColorBrush(item.IsColored ? ColorHelper.FromArgb(0xFF, 0xF6, 0xF6, 0x53) : Colors.Transparent)
            };

            if (item.CheckBoxVisiblity == Visibility.Visible)
            {
                var checkBox = new CheckBox
                {
                    IsChecked = item.IsChecked,
                    VerticalAlignment = VerticalAlignment.Center
                };
                checkBox.Checked += (_, _) =>
                {
                    item.IsChecked = true;
                    item.OnCheckedClick?.Invoke();
                    refresh();
                };
                checkBox.Unchecked += (_, _) =>
                {
                    item.IsChecked = false;
                    item.OnCheckedClick?.Invoke();
                    refresh();
                };
                panel.Children.Add(checkBox);
            }

            panel.Children.Add(new TextBlock
            {
                Text = item.Text,
                VerticalAlignment = VerticalAlignment.Center,
                TextWrapping = TextWrapping.Wrap
            });

            if (item.ConfigButtonVisibility == Visibility.Visible)
            {
                var button = new Button
                {
                    Content = "...",
                    Width = 32,
                    Height = 24,
                    IsEnabled = item.IsConfigEnabled,
                    VerticalAlignment = VerticalAlignment.Center
                };
                button.Click += async (_, _) =>
                {
                    if (item.OnConfigureClickAsync is not null)
                    {
                        await item.OnConfigureClickAsync();
                        refresh();
                    }
                };
                panel.Children.Add(button);
            }

            var node = new TreeViewNode
            {
                Content = panel,
                IsExpanded = true
            };

            foreach (var child in item.Children)
                node.Children.Add(BuildNode(child, refresh));

            return node;
        }
    }
}