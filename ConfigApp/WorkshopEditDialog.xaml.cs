using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows;

namespace ConfigApp
{
    public enum WorkshopEditDialogMode
    {
        Edit,
        Install
    }

    public class WorkshopSubmissionFile : IComparable<WorkshopSubmissionFile>
    {
        public string Name { get; private set; }
        public bool IsEnabled { get; private set; }

        public WorkshopSubmissionFile(string name, bool enabled = true)
        {
            Name = name;
            IsEnabled = enabled;
        }

        public int CompareTo(WorkshopSubmissionFile obj)
        {
            return Name.CompareTo(obj.Name);
        }
    }

    public class WorkshopSubmissionFileState
    {
        public TreeMenuItem Item { get; private set; }
        public string FullPath { get; private set; }

        public WorkshopSubmissionFileState(TreeMenuItem item, string fullPath)
        {
            Item = item;
            FullPath = fullPath;
        }
    }

    public partial class WorkshopEditDialog : Window
    {
        public List<WorkshopSubmissionFileState> FileStates { get; private set; } = new List<WorkshopSubmissionFileState>();

        private WorkshopEditDialogMode m_DialogMode;

        public WorkshopEditDialog(List<WorkshopSubmissionFile> files, WorkshopEditDialogMode dialogMode)
        {
            InitializeComponent();

            m_DialogMode = dialogMode;

            if (dialogMode == WorkshopEditDialogMode.Install)
            {
                Title = "Install Submission";

                files_info_text.Text = "This submission contains the following files";
                install_confirm_text.Visibility = Visibility.Visible;
                button_yes.Visibility = Visibility.Visible;
                button_save_or_no.Content = "No";
            }

            TreeMenuItem generateItem(string text, TreeMenuItem parent = null)
            {
                var item = new TreeMenuItem(text, parent);
                if (dialogMode == WorkshopEditDialogMode.Install)
                {
                    item.CheckBoxVisiblity = Visibility.Collapsed;
                }
                return item;
            }

            TreeMenuItem luaParentItem = generateItem("Scripts");
            TreeMenuItem mp3ParentItem = generateItem("Sounds");
            TreeMenuItem txtParentItem = generateItem("Text Files");

            var parentFolderItems = new Dictionary<string, TreeMenuItem>();

            foreach (var file in files)
            {
                var pathName = file.Name;

                // practically impossible, but I still have gotten errors when trying to substring. So just to be safe :)
                if (pathName.Length - 4 <= 0)
                {
                    continue;
                }

                var pathFragments = pathName.Split('\\');

                TreeMenuItem targetItem;
                switch (pathName.Substring(pathName.Length - 4))
                {
                    case ".lua":
                        targetItem = luaParentItem;
                        break;
                    case ".mp3":
                        targetItem = mp3ParentItem;
                        break;
                    case ".txt":
                        if (dialogMode != WorkshopEditDialogMode.Install)
                        {
                            continue;
                        }
                        targetItem = txtParentItem;
                        break;
                    default:
                        continue;
                }

                if (pathFragments.Length > 1)
                {
                    for (int i = 1; i < pathFragments.Length; i++)
                    {
                        var prevFragment = pathFragments[i - 1];
                        var curFragment = pathFragments[i];

                        if (!parentFolderItems.ContainsKey(prevFragment))
                        {
                            parentFolderItems[prevFragment] = generateItem(prevFragment, targetItem);
                            targetItem.AddChild(parentFolderItems[prevFragment]);
                        }

                        if (i == pathFragments.Length - 1)
                        {
                            targetItem = parentFolderItems[prevFragment];
                        }
                        else
                        {
                            if (!parentFolderItems.ContainsKey(curFragment))
                            {
                                parentFolderItems[curFragment] = generateItem(curFragment, parentFolderItems[prevFragment]);
                                parentFolderItems[prevFragment].AddChild(parentFolderItems[curFragment]);
                            }
                            targetItem = parentFolderItems[curFragment];
                        }
                    }
                }

                var menuItem = generateItem(pathFragments.Last(), targetItem);
                targetItem.AddChild(menuItem);
                FileStates.Add(new WorkshopSubmissionFileState(menuItem, pathName));

                menuItem.IsChecked = file.IsEnabled;
            }

            files_tree_view.Items.Clear();
            files_tree_view.Items.Add(luaParentItem);
            files_tree_view.Items.Add(mp3ParentItem);
            files_tree_view.Items.Add(txtParentItem);

            for (int i = files_tree_view.Items.Count - 1; i >= 0; i--)
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
            if (DialogResult == null)
            {
                DialogResult = false;
            }
        }

        private void yes_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
            Close();
        }

        private void save_or_no_Click(object sender, RoutedEventArgs e)
        {
            DialogResult = m_DialogMode == WorkshopEditDialogMode.Edit;
            Close();
        }
    }
}
