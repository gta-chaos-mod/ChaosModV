using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Settings
{
    public class SoundsTab : Tab
    {
        private CheckBox? m_UseMCI = null;

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

            grid.PushRowSpacedPair("Use legacy MCI for effect sounds", m_UseMCI = Utils.GenerateCommonCheckBox());

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);
        }

        public override void OnLoadValues()
        {
            if (m_UseMCI is not null)
                m_UseMCI.IsChecked = OptionsManager.ConfigFile.ReadValue("EffectSoundUseMCI", false);
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("EffectSoundUseMCI", m_UseMCI?.IsChecked);
        }
    }
}
