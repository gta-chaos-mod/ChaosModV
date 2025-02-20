using System.Windows;
using System.Windows.Controls;
using System.Windows.Markup;

namespace ConfigApp.Tabs
{
    public class MetaTab : Tab
    {
        private TextBox? m_MetaEffectDispatchTimer = null;
        private TextBox? m_MetaEffectDuration = null;
        private TextBox? m_MetaEffectShortDuration = null;

        private static readonly DependencyProperty IsSelectableProperty =
            DependencyProperty.Register("IsSelectable", typeof(bool), typeof(TreeViewItem), new FrameworkPropertyMetadata(true, null) { BindsTwoWayByDefault = true });

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));
            SetRowHeight(new GridLength(80f));

            PushRowTextBlock("Meta effects modify various aspects of mod itself on a regular basis to add more variety." +
                "\nThey are not a part of the normal effects pool; instead, they are automatically dispatched by the mod on a fixed interval (configurable below).");
            PopRow();

            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var grid = new ChaosGrid();
            grid.PushNewColumn(new GridLength(500f));
            grid.PushNewColumn(new GridLength(300f));
            grid.PushNewColumn(new GridLength());

            var settingsGrid = new ChaosGrid();
            settingsGrid.PushNewColumn(new GridLength(330f));
            settingsGrid.PushNewColumn(new GridLength(10f));
            settingsGrid.PushNewColumn(new GridLength());

            settingsGrid.PushRowSpacedPair("How often should a new meta start (in seconds)?", m_MetaEffectDispatchTimer = Utils.GenerateCommonNumericOnlyTextBox());
            settingsGrid.PopRow();

            settingsGrid.PushRowSpacedPair("How long should meta effects last (in seconds)?", m_MetaEffectDuration = Utils.GenerateCommonNumericOnlyTextBox());
            settingsGrid.PopRow();

            settingsGrid.PushRowSpacedPair("How long should short timed meta effects last (in seconds)?", m_MetaEffectShortDuration = Utils.GenerateCommonNumericOnlyTextBox());
            settingsGrid.PopRow();

            grid.PushRowElement(settingsGrid.Grid);

            grid.PushRowEmpty();
            var itemContainerStyle = new Style()
            {
                TargetType = typeof(TreeViewItem)
            };
            itemContainerStyle.Setters.Add(new Setter { Property = IsSelectableProperty, Value = true });
            grid.PushRowElement(new TreeView()
            {
                ItemContainerStyle = itemContainerStyle,
                Resources = (ResourceDictionary)XamlReader.Parse(@"
                <HierarchicalDataTemplate xmlns=""http://schemas.microsoft.com/winfx/2006/xaml/presentation"" xmlns:x=""http://schemas.microsoft.com/winfx/2006/xaml""
                    DataType=""{x:Type TreeMenuItem}"" ItemsSource=""{Binding Children}"">
                    <StackPanel Orientation=""Horizontal"" Height=""20"">
                        <CheckBox Focusable=""False"" IsChecked=""{Binding IsChecked}"" VerticalAlignment=""Center"" />
                        <TextBlock Text=""{Binding Text}"" Margin=""5,0"" VerticalAlignment=""Center"" />
                        <Button Content=""..."" Visibility=""{Binding IsConfigVisible}"" IsEnabled=""{Binding IsConfigEnabled}"" Width=""18"" Height=""18"" Margin=""8,0"" VerticalAlignment=""Center"" Click=""effect_user_config_Click"" />
                    </StackPanel>
                </HierarchicalDataTemplate>
                ")
            });

            PushRowElement(grid.Grid);
        }

        public override void OnLoadValues()
        {
            if (m_MetaEffectDispatchTimer is not null)
                m_MetaEffectDispatchTimer.Text = $"{OptionsManager.ConfigFile.ReadValue("NewMetaEffectSpawnTime", 600)}";
            if (m_MetaEffectDuration is not null)
                m_MetaEffectDuration.Text = $"{OptionsManager.ConfigFile.ReadValue("MetaEffectDur", 95)}";
            if (m_MetaEffectShortDuration is not null)
                m_MetaEffectShortDuration.Text = $"{OptionsManager.ConfigFile.ReadValue("MetaShortEffectDur", 65)}";
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValueAsInt("NewMetaEffectSpawnTime", m_MetaEffectDispatchTimer?.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("MetaEffectDur", m_MetaEffectDuration?.Text);
            OptionsManager.ConfigFile.WriteValueAsInt("MetaShortEffectDur", m_MetaEffectShortDuration?.Text);
        }
    }
}
