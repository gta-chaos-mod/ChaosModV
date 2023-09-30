using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows;
using System.Windows.Input;

namespace ConfigApp
{
    public class TreeMenuItem : INotifyPropertyChanged
    {
        private class TreeMenuItemAction : ICommand
        {
            public event EventHandler CanExecuteChanged;

            private Action m_Action;

            public TreeMenuItemAction(Action action)
            {
                m_Action = action;
            }

            public bool CanExecute(object parameter)
            {
                return true;
            }

            public void Execute(object parameter)
            {
                if (m_Action == null)
                {
                    return;
                }

                m_Action();
            }
        }

        public string Text { get; private set; }
        public string BaseText { get; private set; }
        public TreeMenuItem Parent;
        public List<TreeMenuItem> Children { get; private set; }
        public event PropertyChangedEventHandler PropertyChanged;

        public Visibility CheckBoxVisiblity { get; set; } = Visibility.Visible;

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
        private bool m_isColored;
        public bool IsColored
        {
            get
            {
                return m_isColored;
            }
            set
            {
                m_isColored = value;

                NotifyFieldsUpdated();
            }
        }
        private bool m_ForceConfigHidden = false;
        public string IsConfigVisible
        {
            get
            {
                return Children.Count == 0 && !m_ForceConfigHidden ? "Visible" : "Hidden";
            }
        }
        public bool ForceConfigHidden
        {
            set
            {
                m_ForceConfigHidden = value;
            }
        }
        public bool IsConfigEnabled
        {
            get
            {
                return IsChecked;
            }
        }
        public Action OnConfigureClick { get; set; }
        public ICommand OnConfigureCommand
        {
            get
            {
                return new TreeMenuItemAction(OnConfigureClick);
            }
        }

        public TreeMenuItem(string text, TreeMenuItem parent = null)
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
            int totalChildren = 0, enabledChildren = 0;
            void countChildrenRecursive(TreeMenuItem menuItem)
            {
                if (menuItem.Children.Count == 0)
                {
                    totalChildren++;
                    if (menuItem.IsChecked)
                    {
                        enabledChildren++;
                    }
                }

                foreach (var _menuItem in menuItem.Children)
                {
                    countChildrenRecursive(_menuItem);
                }
            }

            foreach (var menuItem in Children)
            {
                countChildrenRecursive(menuItem);
            }

            if (CheckBoxVisiblity == Visibility.Visible)
            {
                Text = $"{BaseText} ({enabledChildren}/{totalChildren})";
            }
            else
            {
                Text = $"{BaseText} ({totalChildren})";
            }
            m_isChecked = enabledChildren > 0;

            NotifyFieldsUpdated();

            Parent?.UpdateCheckedAccordingToChildrenStatus();
        }

        private void NotifyFieldsUpdated()
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("Text"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsChecked"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigVisible"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsConfigEnabled"));
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs("IsColored"));
        }
    }
}
