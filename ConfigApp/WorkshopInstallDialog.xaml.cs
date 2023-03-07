using System;
using System.Collections.Generic;
using System.Text;
using System.Diagnostics;
using System.Windows;
using System.Windows.Documents;

namespace ConfigApp
{
    public partial class WorkshopInstallDialog : Window
    {
        public WorkshopInstallDialog(List<string> entries)
        {
            Init(entries);
        }

        private void Init(List<string> entries)
        {
            InitializeComponent();

            TreeMenuItem luaParentItem = new TreeMenuItem("Lua Scripts");
            TreeMenuItem mp3ParentItem = new TreeMenuItem("Effect Sounds");
            TreeMenuItem txtParentItem = new TreeMenuItem("Text Files");

            Trace.WriteLine(entries.Count);

            foreach (string file in entries)
            {
                if (file.Length - 4 <= 0) continue; // practically impossible, but I still have gotten errors when trying to substring. So just to be safe :)
                
                TreeMenuItem menuItem = new TreeMenuItem(file);
                switch (file.Substring(file.Length - 4))
                {
                    case ".lua":
                        luaParentItem.AddChild(menuItem);
                        break;
                    case ".mp3":
                        mp3ParentItem.AddChild(menuItem);
                        break;
                    case ".txt":
                        txtParentItem.AddChild(menuItem);
                        break;
                }
            }

            files_tree_view.Items.Clear();
            files_tree_view.Items.Add(luaParentItem);
            files_tree_view.Items.Add(mp3ParentItem);
            files_tree_view.Items.Add(txtParentItem);

            for (int i = files_tree_view.Items.Count-1; i >= 0; i--)
            {
                TreeMenuItem child = (TreeMenuItem)files_tree_view.Items.GetItemAt(i);
                if (child.Children.Count == 0)
                {
                    files_tree_view.Items.RemoveAt(i);
                }
            }
        }

        private void OnWindowClosed(object sender, EventArgs e)
        {
            if (this.DialogResult == null) this.DialogResult = false;
        }

        private void BtnYesClick(object sender, RoutedEventArgs e)
        {
            this.DialogResult = true;
            this.Close();
        }

        private void BtnNoClick(object sender, RoutedEventArgs e)
        {
            this.DialogResult = false;
            this.Close();
        }
    }
}
