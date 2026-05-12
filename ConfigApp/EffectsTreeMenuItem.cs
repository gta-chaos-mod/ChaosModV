using System.ComponentModel;
using Microsoft.UI.Xaml;

namespace ConfigApp
{
    public class TreeMenuItem : INotifyPropertyChanged
    {
        public string Text { get; private set; }
        public string BaseText { get; private set; }
        public TreeMenuItem? Parent = null;
        public List<TreeMenuItem> Children { get; private set; }
        public event PropertyChangedEventHandler? PropertyChanged;

        public Visibility CheckBoxVisibility { get; set; } = Visibility.Visible;

        [Obsolete("Use CheckBoxVisibility instead.")]
        public Visibility CheckBoxVisiblity
        {
            get => CheckBoxVisibility;
            set => CheckBoxVisibility = value;
        }

        private bool m_isChecked;
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

        private bool m_isColored;
        public bool IsColored
        {
            get => m_isColored;
            set
            {
                m_isColored = value;
                NotifyFieldsUpdated();
            }
        }

        private bool m_ForceConfigHidden = false;
        public Visibility ConfigButtonVisibility => Children.Count == 0 && !m_ForceConfigHidden ? Visibility.Visible : Visibility.Collapsed;
        public bool ForceConfigHidden
        {
            set => m_ForceConfigHidden = value;
        }
        public bool IsConfigEnabled => IsChecked;
        public Func<Task>? OnConfigureClickAsync { get; set; }
        public Action? OnCheckedClick { get; set; }

        public TreeMenuItem(string text, TreeMenuItem? parent = null)
        {
            Text = text;
            BaseText = text;
            Parent = parent;
            Children = new List<TreeMenuItem>();
            m_isChecked = false;
        }

        public void AddChild(TreeMenuItem menuItem)
        {
            if (menuItem != null)
            {
                menuItem.Parent = this;
                Children.Add(menuItem);
            }
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
                    return;
                }

                foreach (var child in menuItem.Children)
                    CountRecursive(child);
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
