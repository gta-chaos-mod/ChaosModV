using System.ComponentModel;
using Microsoft.UI.Xaml;

namespace ConfigApp
{
    internal class TreeMenuItem : INotifyPropertyChanged
    {
        private bool m_isChecked;
        private bool m_isColored;
        private bool m_forceConfigHidden;

        public string Text { get; private set; }
        public string BaseText { get; }
        public TreeMenuItem? Parent { get; set; }
        public List<TreeMenuItem> Children { get; }
        public event PropertyChangedEventHandler? PropertyChanged;

        public Visibility CheckBoxVisibility { get; set; } = Visibility.Visible;

        public bool IsChecked
        {
            get => m_isChecked;
            set
            {
                m_isChecked = value;
                NotifyFieldsUpdated();

                foreach (var menuItem in Children)
                {
                    menuItem.IsChecked = value;
                    menuItem.OnCheckedClick?.Invoke();
                }

                Parent?.UpdateCheckedAccordingToChildrenStatus();
            }
        }

        public bool IsColored
        {
            get => m_isColored;
            set
            {
                m_isColored = value;
                NotifyFieldsUpdated();
            }
        }

        public Visibility ConfigButtonVisibility => (Children.Count == 0 && !m_forceConfigHidden) ? Visibility.Visible : Visibility.Collapsed;
        public bool IsConfigEnabled => IsChecked;
        public Func<Task>? OnConfigureClickAsync { get; set; }
        public Action? OnCheckedClick { get; set; }

        public TreeMenuItem(string text, TreeMenuItem? parent = null)
        {
            Text = text;
            BaseText = text;
            Parent = parent;
            Children = new List<TreeMenuItem>();
        }

        public void AddChild(TreeMenuItem menuItem)
        {
            menuItem.Parent = this;
            Children.Add(menuItem);
        }

        public void SetForceConfigHidden(bool value)
        {
            m_forceConfigHidden = value;
        }

        public void UpdateCheckedAccordingToChildrenStatus()
        {
            var (totalChildren, enabledChildren) = CountLeafChildren();

            Text = CheckBoxVisibility == Visibility.Visible
                ? $"{BaseText} ({enabledChildren}/{totalChildren})"
                : $"{BaseText} ({totalChildren})";

            m_isChecked = enabledChildren > 0;
            NotifyFieldsUpdated();
            Parent?.UpdateCheckedAccordingToChildrenStatus();
        }

        private (int Total, int Enabled) CountLeafChildren()
        {
            var totalChildren = 0;
            var enabledChildren = 0;

            void CountRecursive(TreeMenuItem menuItem)
            {
                if (menuItem.Children.Count == 0)
                {
                    totalChildren++;
                    if (menuItem.IsChecked)
                        enabledChildren++;
                }
                else
                {
                    foreach (var child in menuItem.Children)
                        CountRecursive(child);
                }
            }

            foreach (var menuItem in Children)
                CountRecursive(menuItem);

            return (totalChildren, enabledChildren);
        }

        private void NotifyFieldsUpdated()
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(Text)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsChecked)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(ConfigButtonVisibility)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsConfigEnabled)));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(IsColored)));
        }
    }
}
