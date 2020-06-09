using System.Collections.Generic;
using System.ComponentModel;

namespace ConfigApp
{
    public class TreeMenuItem : INotifyPropertyChanged
    {
        public string Text { get; private set; }
        public TreeMenuItem Parent;
        public List<TreeMenuItem> Children { get; private set; }
        public event PropertyChangedEventHandler PropertyChanged;
        private bool m_isChecked;
        public bool IsChecked
        {
            get
            {
                return m_isChecked;
            }
            set
            {
                m_isChecked = value;

                NotifyFieldsUpdated();

                foreach (TreeMenuItem menuItem in Children)
                {
                    menuItem.IsChecked = value;
                }

                Parent?.UpdateCheckedAccordingToChildrenStatus();
            }
        }
        public string IsConfigVisible
        {
            get
            {
                return Children.Count == 0 ? "Visible" : "Hidden";
            }
        }
        public bool IsConfigEnabled
        {
            get
            {
                return IsChecked;
            }
        }

        public TreeMenuItem(string text, TreeMenuItem parent = null)
        {
            Text = text;
            Parent = parent;
            Children = new List<TreeMenuItem>();
            m_isChecked = true;
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
            bool shouldBeChecked = false;
            foreach (TreeMenuItem menuItem in Children)
            {
                if (menuItem.IsChecked)
                {
                    shouldBeChecked = true;
                    break;
                }
            }

            m_isChecked = shouldBeChecked;

            NotifyFieldsUpdated();
        }

        private void NotifyFieldsUpdated()
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsChecked"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigVisible"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigEnabled"));
        }
    }
}
