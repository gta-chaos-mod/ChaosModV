using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Settings
{
    public class DistanceTab : Tab
    {
        private CheckBox? m_EnableDistanceBasedDispatch = null;
        private TextBox? m_DistanceBasedDispatchDistance = null;
        private ComboBox? m_DistanceBasedDispatchType = null;
        private CheckBox? m_EnableCrossingChallenge = null;

        private void SetDistanceDispatchFieldsEnabled(bool state)
        {
            if (m_DistanceBasedDispatchDistance is not null)
                m_DistanceBasedDispatchDistance.IsEnabled = state;
            if (m_DistanceBasedDispatchType is not null)
                m_DistanceBasedDispatchType.IsEnabled = state;
        }

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));
            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var scrollViewer = new ScrollViewer();

            var grid = new ChaosGrid();
            grid.PushNewColumn(new GridLength(310f));
            grid.PushNewColumn(new GridLength(10f));
            grid.PushNewColumn(new GridLength(100f));
            grid.PushNewColumn(new GridLength(450f));
            grid.PushNewColumn(new GridLength(10f));
            grid.PushNewColumn(new GridLength());

            m_EnableDistanceBasedDispatch = Utils.GenerateCommonCheckBox();
            m_EnableDistanceBasedDispatch.Click += (sender, eventArgs) =>
            {
                SetDistanceDispatchFieldsEnabled(m_EnableDistanceBasedDispatch.IsChecked.GetValueOrDefault());
            };
            grid.PushRowSpacedPair("Enable distance-based effect dispatch", m_EnableDistanceBasedDispatch);
            grid.PopRow();

            grid.PushRowSpacedPair("Distance to activate effect (in meters)", m_DistanceBasedDispatchDistance = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PushRowSpacedPair("Type of travel distance", m_DistanceBasedDispatchType = new ComboBox()
            {
                Width = 120f,
                Height = 25f,
                ItemsSource = new string[]
                {
                    "Distance",
                    "Displacement"
                }
            });
            grid.PopRow();

            grid.PushRowSpacedPair("Enable Crossing Challenge™", m_EnableCrossingChallenge = Utils.GenerateCommonCheckBox());

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);

            SetDistanceDispatchFieldsEnabled(false);
        }

        public override void OnLoadValues()
        {
            if (m_EnableDistanceBasedDispatch is not null)
            {
                m_EnableDistanceBasedDispatch.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableDistanceBasedEffectDispatch", false);
                SetDistanceDispatchFieldsEnabled(m_EnableDistanceBasedDispatch.IsChecked.GetValueOrDefault());
            }
            if (m_DistanceBasedDispatchDistance is not null)
                m_DistanceBasedDispatchDistance.Text = OptionsManager.ConfigFile.ReadValue("DistanceToActivateEffect", "250");
            if (m_DistanceBasedDispatchType is not null)
                m_DistanceBasedDispatchType.SelectedIndex = OptionsManager.ConfigFile.ReadValueInt("DistanceType", 0);
            if (m_EnableCrossingChallenge is not null)
                m_EnableCrossingChallenge.IsChecked = OptionsManager.ConfigFile.ReadValueBool("EnableCrossingChallenge", false);
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("EnableDistanceBasedEffectDispatch", m_EnableDistanceBasedDispatch?.IsChecked);
            OptionsManager.ConfigFile.WriteValue("DistanceToActivateEffect", m_DistanceBasedDispatchDistance?.Text);
            OptionsManager.ConfigFile.WriteValue("DistanceType", m_DistanceBasedDispatchType?.SelectedIndex);
            OptionsManager.ConfigFile.WriteValue("EnableCrossingChallenge", m_EnableCrossingChallenge?.IsChecked);
        }
    }
}
