using System.ComponentModel;
using ConfigApp.Infrastructure;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

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
        public string Name { get; }
        public bool IsEnabled { get; }
        public WorkshopSubmissionFileType Type { get; }
        public EffectData? EffectData { get; }

        public WorkshopSubmissionFile(string name, bool enabled, EffectData? effectData = null)
        {
            Name = name;
            IsEnabled = enabled;
            Type = GetFileType(name);
            EffectData = effectData;
        }

        public int CompareTo(WorkshopSubmissionFile? obj)
        {
            if (obj is null)
                throw new ArgumentNullException(nameof(obj));

            return StringComparer.OrdinalIgnoreCase.Compare(Name, obj.Name);
        }

        private static WorkshopSubmissionFileType GetFileType(string name)
        {
            return Path.GetExtension(name).ToLowerInvariant() switch
            {
                ".lua" => WorkshopSubmissionFileType.Script,
                ".mp3" => WorkshopSubmissionFileType.Sound,
                ".txt" => WorkshopSubmissionFileType.Text,
                _ => WorkshopSubmissionFileType.Undefined
            };
        }
    }

    public class WorkshopSubmissionFileState
    {
        public TreeMenuItem Item { get; }
        public string FullPath { get; }
        public EffectData? EffectData { get; set; }

        public WorkshopSubmissionFileState(TreeMenuItem item, string fullPath, EffectData? effectData = null)
        {
            Item = item;
            FullPath = fullPath;
            EffectData = effectData;
        }
    }

    public partial class WorkshopEditDialog : ContentDialog
    {
        public List<WorkshopSubmissionFileState> FileStates { get; } = new();

        private readonly WorkshopEditDialogMode m_DialogMode;
        private readonly List<TreeMenuItem> m_RootItems = new();

        public WorkshopEditDialog(List<WorkshopSubmissionFile> files, WorkshopEditDialogMode dialogMode, string? path = null, List<string>? highlightedFiles = null)
        {
            InitializeComponent();
            AppDialog.ApplyToDialog(this);

            m_DialogMode = dialogMode;
            DefaultButton = ContentDialogButton.Primary;
            ConfigureDialogTexts();
            BuildTree(files, path, highlightedFiles);
            RefreshTree();
        }

        private void ConfigureDialogTexts()
        {
            if (m_DialogMode == WorkshopEditDialogMode.Install)
            {
                Title = "Install Submission";
                PrimaryButtonText = "Yes";
                CloseButtonText = "No";
                files_info_text.Text = "This submission contains the following files";
                install_confirm_text.Visibility = Visibility.Visible;
                return;
            }

            Title = "Edit Submission";
            PrimaryButtonText = "Save";
            CloseButtonText = "Cancel";
        }

        private void BuildTree(List<WorkshopSubmissionFile> files, string? path, List<string>? highlightedFiles)
        {
            var rootItems = new Dictionary<WorkshopSubmissionFileType, TreeMenuItem>
            {
                [WorkshopSubmissionFileType.Script] = CreateRootItem("Scripts"),
                [WorkshopSubmissionFileType.Sound] = CreateRootItem("Sounds"),
                [WorkshopSubmissionFileType.Text] = CreateRootItem("Text Files", showCheckbox: false)
            };

            var folderCache = new Dictionary<string, TreeMenuItem>(StringComparer.OrdinalIgnoreCase);

            foreach (var file in files)
            {
                if (!TryGetBaseTarget(file.Type, rootItems, out var targetRoot))
                    continue;

                var pathName = file.Name;
                if (pathName.Length < 4)
                    continue;

                var pathFragments = NormalizePath(pathName).Split('\\', StringSplitOptions.RemoveEmptyEntries);
                var targetItem = CreateFolderHierarchy(pathFragments, targetRoot, folderCache, file.Type);
                var menuItem = CreateLeafItem(pathFragments.Last(), targetItem, file.Type != WorkshopSubmissionFileType.Text, file, pathName, path, highlightedFiles);

                targetItem.AddChild(menuItem);
                FileStates.Add(new WorkshopSubmissionFileState(menuItem, pathName, file.EffectData));
            }

            m_RootItems.Clear();
            foreach (var item in rootItems.Values)
            {
                if (item.Children.Count > 0)
                    m_RootItems.Add(item);
            }
        }

        private static string NormalizePath(string path)
        {
            return path.StartsWith("sounds\\", StringComparison.OrdinalIgnoreCase) ? path[7..] : path;
        }

        private static bool TryGetBaseTarget(WorkshopSubmissionFileType fileType, IReadOnlyDictionary<WorkshopSubmissionFileType, TreeMenuItem> rootItems, out TreeMenuItem target)
        {
            if (rootItems.TryGetValue(fileType, out target!))
                return true;

            target = null!;
            return false;
        }

        private TreeMenuItem CreateRootItem(string text, bool showCheckbox = true)
        {
            return CreateTreeItem(text, null, showCheckbox);
        }

        private TreeMenuItem CreateFolderItem(string text, TreeMenuItem parent)
        {
            return CreateTreeItem(text, parent, showCheckbox: m_DialogMode == WorkshopEditDialogMode.Edit);
        }

        private TreeMenuItem CreateTreeItem(string text, TreeMenuItem? parent, bool showCheckbox)
        {
            var item = new TreeMenuItem(text, parent);
            if (m_DialogMode == WorkshopEditDialogMode.Install || !showCheckbox)
                item.CheckBoxVisibility = Visibility.Collapsed;
            return item;
        }

        private TreeMenuItem CreateItem(string text, TreeMenuItem? parent = null, bool showCheckbox = true)
        {
            var item = new TreeMenuItem(text, parent);
            if (m_DialogMode == WorkshopEditDialogMode.Install || !showCheckbox)
                item.CheckBoxVisibility = Visibility.Collapsed;
            return item;
        }

        private TreeMenuItem CreateFolderHierarchy(string[] fragments, TreeMenuItem root, Dictionary<string, TreeMenuItem> folderCache, WorkshopSubmissionFileType fileType)
        {
            var current = root;
            if (fragments.Length <= 1)
                return current;

            for (var i = 0; i < fragments.Length - 1; i++)
            {
                var folderPath = string.Join(Path.DirectorySeparatorChar.ToString(), fragments.Take(i + 1));
                if (!folderCache.TryGetValue(folderPath, out var folderItem))
                {
                    folderItem = CreateFolderItem(GetDisplayName(fragments[i], fileType), current);
                    folderCache[folderPath] = folderItem;
                    current.AddChild(folderItem);
                }

                current = folderItem;
            }

            return current;
        }

        private TreeMenuItem CreateLeafItem(string text, TreeMenuItem parent, bool showCheckbox, WorkshopSubmissionFile file, string pathName, string? path, List<string>? highlightedFiles)
        {
            var menuItem = CreateTreeItem(GetDisplayName(text, file.Type), parent, showCheckbox);
            menuItem.ForceConfigHidden = m_DialogMode != WorkshopEditDialogMode.Edit;
            menuItem.OnConfigureClickAsync = CreateConfigureHandler(file, pathName, path, menuItem);

            if (highlightedFiles?.Contains(pathName) ?? false)
                menuItem.IsColored = true;

            menuItem.IsChecked = file.IsEnabled;
            return menuItem;
        }

        private Func<Task> CreateConfigureHandler(WorkshopSubmissionFile file, string pathName, string? path, TreeMenuItem menuItem)
        {
            return async () =>
            {
                if (file.Type == WorkshopSubmissionFileType.Script)
                {
                    var effectConfig = new EffectConfig(null, menuItem.Parent?.Children.Count > 0 ? null : file.EffectData, new Effects.EffectInfo
                    {
                        Name = pathName,
                        IsTimed = true
                    });
                    await effectConfig.ShowAsync();

                    if (!effectConfig.IsSaved)
                        return;

                    FileStates.First(state => state.Item == menuItem).EffectData = effectConfig.GetNewData();
                    return;
                }

                try
                {
                    var fullPath = path is not null ? Path.Combine(path.Replace('/', '\\'), pathName) : pathName;
                    Utils.OpenURL(fullPath);
                }
                catch (Win32Exception)
                {
                    await AppDialog.ShowMessageAsync("Error: File not found");
                }
            };
        }

        private static string GetDisplayName(string name, WorkshopSubmissionFileType fileType)
        {
            if (fileType != WorkshopSubmissionFileType.Sound)
                return name;

            var effectId = name;
            if (!Effects.EffectsMap.ContainsKey(effectId) && name.Length > 4)
                effectId = name[..^4];

            return Effects.EffectsMap.ContainsKey(effectId)
                ? $"{name} ({Effects.EffectsMap[effectId].Name})"
                : name;
        }

        private void RefreshTree()
        {
            TreeViewBuilder.Populate(files_tree_view, m_RootItems, RefreshTree);
        }
    }
}
