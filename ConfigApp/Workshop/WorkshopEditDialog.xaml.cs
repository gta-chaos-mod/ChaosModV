using System.ComponentModel;
using System.Diagnostics;
using System.Windows;

namespace ConfigApp
{
    public enum WorkshopEditDialogMode
    {
        Edit,
        Install
    }

    public enum WorkshopSubmissionFileType
    {
        Script,
        Sound,
        Text,
        Undefined
    }

    public class WorkshopSubmissionFile : IComparable<WorkshopSubmissionFile>
    {
        public string Name { get; private set; }
        public bool IsEnabled { get; private set; }
        public WorkshopSubmissionFileType Type { get; private set; }
        public EffectData? EffectData { get; private set; }

        public WorkshopSubmissionFile(string name, bool enabled, EffectData? effectData = null)
        {
            Name = name;
            switch (name[^4..])
            {
            case ".lua":
                Type = WorkshopSubmissionFileType.Script;
                break;
            case ".mp3":
                Type = WorkshopSubmissionFileType.Sound;
                break;
            case ".txt":
                Type = WorkshopSubmissionFileType.Text;
                break;
            default:
                Type = WorkshopSubmissionFileType.Undefined;
                break;
            }
            IsEnabled = enabled;
            EffectData = effectData;
        }

        public int CompareTo(WorkshopSubmissionFile? obj)
        {
            if (obj is null)
                throw new ArgumentNullException(nameof(obj));

            return Name.CompareTo(obj.Name);
        }
    }

    public class WorkshopSubmissionFileState
    {
        public TreeMenuItem Item { get; private set; }
        public string FullPath { get; private set; }
        public EffectData? EffectData { get; set; }

        public WorkshopSubmissionFileState(TreeMenuItem item, string fullPath, EffectData? effectData = null)
        {
            Item = item;
            FullPath = fullPath;
            EffectData = effectData;
        }
    }

    public partial class WorkshopEditDialog : Window
    {
        public List<WorkshopSubmissionFileState> FileStates { get; private set; } = new List<WorkshopSubmissionFileState>();

        private readonly WorkshopEditDialogMode m_DialogMode;

        public WorkshopEditDialog(List<WorkshopSubmissionFile> files, WorkshopEditDialogMode dialogMode, string? path = null, List<string>? highlightedFiles = null)
        {
            InitializeComponent();

            m_DialogMode = dialogMode;

            if (m_DialogMode == WorkshopEditDialogMode.Install)
            {
                Title = "Install Submission";

                files_info_text.Text = "This submission contains the following files";
                install_confirm_text.Visibility = Visibility.Visible;
                button_yes.Visibility = Visibility.Visible;
                button_save_or_no.Content = "No";
            }

            TreeMenuItem generateItem(string text, TreeMenuItem? parent = null, bool showCheckbox = true)
            {
                var item = new TreeMenuItem(text, parent);
                if (m_DialogMode == WorkshopEditDialogMode.Install || !showCheckbox)
                    item.CheckBoxVisiblity = Visibility.Collapsed;
                return item;
            }

            var luaParentItem = generateItem("Scripts");
            var mp3ParentItem = generateItem("Sounds");
            var txtParentItem = generateItem("Text Files", showCheckbox: false);

            var parentFolderItems = new Dictionary<string, TreeMenuItem>();

            foreach (var file in files)
            {
                var pathName = file.Name;

                // practically impossible, but I still have gotten errors when trying to substring. So just to be safe :)
                if (pathName.Length - 4 <= 0)
                    continue;

                var pathFragments = (pathName.StartsWith("sounds\\") ? pathName[7..] : pathName).Split('\\');

                TreeMenuItem targetItem;
                switch (file.Type)
                {
                case WorkshopSubmissionFileType.Script:
                    targetItem = luaParentItem;
                    break;
                case WorkshopSubmissionFileType.Sound:
                    targetItem = mp3ParentItem;
                    break;
                case WorkshopSubmissionFileType.Text:
                    targetItem = txtParentItem;
                    break;
                default:
                    continue;
                }

                string handleEffectId(string itemName)
                {
                    if (file.Type == WorkshopSubmissionFileType.Sound)
                    {
                        var effectId = itemName;
                        if (!Effects.EffectsMap.ContainsKey(effectId) && itemName.Length > 4)
                            effectId = itemName[..^4];
                        if (Effects.EffectsMap.ContainsKey(effectId))
                            itemName += $" ({Effects.EffectsMap[effectId].Name})";
                    }

                    return itemName;
                }

                if (pathFragments.Length > 1)
                {
                    for (int i = 1; i < pathFragments.Length; i++)
                    {
                        var prevFragment = pathFragments[i - 1];
                        var curFragment = pathFragments[i];

                        if (!parentFolderItems.ContainsKey(prevFragment))
                        {
                            parentFolderItems[prevFragment] = generateItem(handleEffectId(prevFragment), targetItem);
                            targetItem.AddChild(parentFolderItems[prevFragment]);
                        }

                        if (i == pathFragments.Length - 1)
                            targetItem = parentFolderItems[prevFragment];
                        else
                        {
                            if (!parentFolderItems.ContainsKey(curFragment))
                            {
                                parentFolderItems[curFragment] = generateItem(handleEffectId(curFragment), parentFolderItems[prevFragment]);
                                parentFolderItems[prevFragment].AddChild(parentFolderItems[curFragment]);
                            }
                            targetItem = parentFolderItems[curFragment];
                        }
                    }
                }

                var menuItem = generateItem(handleEffectId(pathFragments.Last()), targetItem, file.Type != WorkshopSubmissionFileType.Text);
                var fileState = new WorkshopSubmissionFileState(menuItem, pathName, file.EffectData);
                menuItem.ForceConfigHidden = m_DialogMode != WorkshopEditDialogMode.Edit;
                menuItem.OnConfigureClick = () =>
                {
                    if (file.Type == WorkshopSubmissionFileType.Script)
                    {
                        var effectConfig = new EffectConfig(null, fileState.EffectData, new Effects.EffectInfo()
                        {
                            Name = pathName,
                            IsTimed = true
                        });
                        effectConfig.ShowDialog();

                        if (!effectConfig.IsSaved)
                            return;

                        fileState.EffectData = effectConfig.GetNewData();
                    }
                    else
                    {
                        try
                        {
                            System.Diagnostics.Process.Start(new ProcessStartInfo(path is not null ? path.Replace('/', '\\') + pathName : pathName) { UseShellExecute = true });
                        }
                        catch (Win32Exception)
                        {
                            MessageBox.Show("Error: File not found", "ChaosModV", MessageBoxButton.OK, MessageBoxImage.Error);
                        }
                    }
                };

                if (highlightedFiles?.Contains(pathName) ?? false)
                {
                    menuItem.IsColored = true;
                }

                targetItem.AddChild(menuItem);
                FileStates.Add(fileState);

                menuItem.IsChecked = file.IsEnabled;
            }

            files_tree_view.Items.Clear();
            files_tree_view.Items.Add(luaParentItem);
            files_tree_view.Items.Add(mp3ParentItem);
            files_tree_view.Items.Add(txtParentItem);

            for (int i = files_tree_view.Items.Count - 1; i >= 0; i--)
            {
                var child = (TreeMenuItem)files_tree_view.Items.GetItemAt(i);
                if (child.Children.Count == 0)
                    files_tree_view.Items.RemoveAt(i);
            }
        }

        private void OnWindowClosed(object sender, EventArgs e)
        {
            DialogResult ??= false;
        }

        private void OnWorkshopEditYesClick(object sender, RoutedEventArgs e)
        {
            DialogResult = true;
            Close();
        }

        private void OnWorkshopEditNoOrSaveClick(object sender, RoutedEventArgs e)
        {
            DialogResult = m_DialogMode == WorkshopEditDialogMode.Edit;
            Close();
        }
    }
}
