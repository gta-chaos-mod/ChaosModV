using System;
using System.Collections.Generic;
using System.ComponentModel;

namespace ConfigApp
{
    public class TreeMenuItem : INotifyPropertyChanged
    {
        public string Text { get; private set; }
        public string BaseText { get; private set; }
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
            BaseText = text;
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
            int childsEnabled = 0;
            foreach (TreeMenuItem menuItem in Children)
            {
                if (menuItem.IsChecked)
                {
                    childsEnabled += 1;
                    shouldBeChecked = true;
                }
            }
            this.Text = String.Format("{0} ({1}/{2})", this.BaseText, childsEnabled, this.Children.Count);
            m_isChecked = shouldBeChecked;

            NotifyFieldsUpdated();
        }

        private void NotifyFieldsUpdated()
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Text"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsChecked"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigVisible"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigEnabled"));
        }
    }
}
