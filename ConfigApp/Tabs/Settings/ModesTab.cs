using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Settings
{
    public class ModesTab : Tab
    {
        private enum DispatchModeType
        {
            Time,
            Distance
        };
        private static readonly Dictionary<DispatchModeType, string> DispatchModes = new()
        {
            { DispatchModeType.Time, "Time"},
            { DispatchModeType.Distance, "Distance" }
        };

        private ComboBox? m_DispatchMode = null;

        private ChaosGrid? m_TimeGrid = null;
        private TextBox? m_EffectDispatchTimer = null;
        private TextBox? m_TimedEffectDuration = null;
        private TextBox? m_ShortTimedEffectDuration = null;

        private ChaosGrid? m_DistanceGrid = null;
        private TextBox? m_DistanceBasedDispatchDistance = null;
        private ComboBox? m_DistanceBasedDispatchType = null;

        private CheckBox? m_EnableCrossingChallenge = null;

        private void UpdateDispatchModeGridVisibility()
        {
            if (m_DispatchMode is null || m_TimeGrid is null || m_DistanceGrid is null)
                return;

            switch ((DispatchModeType)m_DispatchMode.SelectedIndex)
            {
            case DispatchModeType.Time:
                m_TimeGrid.Grid.Visibility = Visibility.Visible;
                m_DistanceGrid.Grid.Visibility = Visibility.Collapsed;
                break;
            case DispatchModeType.Distance:
                m_TimeGrid.Grid.Visibility = Visibility.Collapsed;
                m_DistanceGrid.Grid.Visibility = Visibility.Visible;
                break;
            default:
                break;
            }
        }

        protected override void InitContent()
        {
            static ChaosGrid createCommonGrid()
            {
                var grid = new ChaosGrid();
                grid.PushNewColumn(new GridLength(310f));
                grid.PushNewColumn(new GridLength(10f));
                grid.PushNewColumn(new GridLength(100f));
                grid.PushNewColumn(new GridLength(450f));
                grid.PushNewColumn(new GridLength(10f));
                grid.PushNewColumn(new GridLength());
                return grid;
            }

            PushNewColumn(new GridLength(1f, GridUnitType.Star));
            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var scrollViewer = new ScrollViewer();

            var grid = new ChaosGrid();
            grid.PushNewColumn(new GridLength());

            var headerGrid = createCommonGrid();
            headerGrid.PushRowSpacedPair("Effect dispatch mode", m_DispatchMode = new ComboBox()
            {
                Width = 80f,
                Height = 25f,
                ItemsSource = DispatchModes.Values
            });
            m_DispatchMode.SelectionChanged += (sender, args) => UpdateDispatchModeGridVisibility();
            grid.PushRowElement(headerGrid.Grid);
            grid.PopRow();

            var body = new StackPanel();
            grid.SetRowHeight(new GridLength(100f));
            grid.PushRowElement(body);
            grid.PopRow();

            m_TimeGrid = createCommonGrid();
            m_TimeGrid.PushRowSpacedPair("New effect timer (in seconds)", m_EffectDispatchTimer = Utils.GenerateCommonNumericOnlyTextBox());
            m_TimeGrid.PushRowSpacedPair("Timed effect duration (in seconds)", m_TimedEffectDuration = Utils.GenerateCommonNumericOnlyTextBox());
            m_TimeGrid.PopRow();
            m_TimeGrid.PushRowSpacedPair("Short timed effect duration (in seconds)", m_ShortTimedEffectDuration = Utils.GenerateCommonNumericOnlyTextBox());
            body.Children.Add(m_TimeGrid.Grid);

            m_DistanceGrid = createCommonGrid();
            m_DistanceGrid.PushRowSpacedPair("Distance to activate effect (in meters)", m_DistanceBasedDispatchDistance = Utils.GenerateCommonNumericOnlyTextBox());
            m_DistanceGrid.PushRowSpacedPair("Type of travel distance", m_DistanceBasedDispatchType = new ComboBox()
            {
                Width = 120f,
                Height = 25f,
                ItemsSource = new string[]
                {
                        "Distance",
                        "Displacement"
                }
            });
            body.Children.Add(m_DistanceGrid.Grid);

            var footerGrid = createCommonGrid();
            footerGrid.PushRowSpacedPair("Enable Crossing Challenge™", m_EnableCrossingChallenge = Utils.GenerateCommonCheckBox());
            grid.PushRowElement(footerGrid.Grid);

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);
        }

        public override void OnLoadValues()
        {
            if (m_DispatchMode is not null)
            {
                m_DispatchMode.SelectedIndex = !OptionsManager.ConfigFile.ReadValueBool("EnableDistanceBasedEffectDispatch", false) ? 0 : 1;
                UpdateDispatchModeGridVisibility();
            }
            if (m_EffectDispatchTimer is not null)
                m_EffectDispatchTimer.Text = OptionsManager.ConfigFile.ReadValue("NewEffectSpawnTime", "30");
            if (m_TimedEffectDuration is not null)
                m_TimedEffectDuration.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedDur", "90");
            if (m_ShortTimedEffectDuration is not null)
                m_ShortTimedEffectDuration.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedShortDur", "30");
            if (m_DistanceBasedDispatchDistance is not null)
                m_DistanceBasedDispatchDistance.Text = OptionsManager.ConfigFile.ReadValue("DistanceToActivateEffect", "250");
            if (m_DistanceBasedDispatchType is not null)
                m_DistanceBasedDispatchType.SelectedIndex = OptionsManager.ConfigFile.ReadValueInt("DistanceType", 0);
            if (m_EnableCrossingChallenge is not null)
                m_EnableCrossingChallenge.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableCrossingChallenge", false);
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("NewEffectSpawnTime", m_EffectDispatchTimer?.Text);
            OptionsManager.ConfigFile.WriteValue("EffectTimedDur", m_TimedEffectDuration?.Text);
            OptionsManager.ConfigFile.WriteValue("EffectTimedShortDur", m_ShortTimedEffectDuration?.Text);
            OptionsManager.ConfigFile.WriteValue("EnableDistanceBasedEffectDispatch", m_DispatchMode?.SelectedIndex);
            OptionsManager.ConfigFile.WriteValue("DistanceToActivateEffect", m_DistanceBasedDispatchDistance?.Text);
            OptionsManager.ConfigFile.WriteValue("DistanceType", m_DistanceBasedDispatchType?.SelectedIndex);
            OptionsManager.ConfigFile.WriteValue("EnableCrossingChallenge", m_EnableCrossingChallenge?.IsChecked);
        }
    }
}
