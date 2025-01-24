using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs.Settings
{
    public class TimersTab : Tab
    {
        private TextBox? m_EffectDispatchTimer = null;
        private TextBox? m_TimedEffectDuration = null;
        private TextBox? m_ShortTimedEffectDuration = null;

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

            grid.PushRowSpacedPair("New effect timer (in seconds)", m_EffectDispatchTimer = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PushRowSpacedPair("Timed effect duration (in seconds)", m_TimedEffectDuration = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PopRow();

            grid.PushRowSpacedPair("Short timed effect duration (in seconds)", m_ShortTimedEffectDuration = Utils.GenerateCommonNumericOnlyTextBox());
            grid.PopRow();

            scrollViewer.Content = grid.Grid;

            PushRowElement(scrollViewer);
        }

        public override void OnLoadValues()
        {
            if (m_EffectDispatchTimer is not null)
                m_EffectDispatchTimer.Text = OptionsManager.ConfigFile.ReadValue("NewEffectSpawnTime", "30");
            if (m_TimedEffectDuration is not null)
                m_TimedEffectDuration.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedDur", "90");
            if (m_ShortTimedEffectDuration is not null)
                m_ShortTimedEffectDuration.Text = OptionsManager.ConfigFile.ReadValue("EffectTimedShortDur", "30");
        }

        public override void OnSaveValues()
        {
            OptionsManager.ConfigFile.WriteValue("NewEffectSpawnTime", m_EffectDispatchTimer?.Text);
            OptionsManager.ConfigFile.WriteValue("EffectTimedDur", m_TimedEffectDuration?.Text);
            OptionsManager.ConfigFile.WriteValue("EffectTimedShortDur", m_ShortTimedEffectDuration?.Text);
        }
    }
}
