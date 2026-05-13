using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs
{
    public sealed partial class MoreTab : UserControl, ITabLifecycle
    {
        public MoreTab()
        {
            InitializeComponent();
        }

        public void OnTabSelected()
        {
        }

        public void OnLoadValues()
        {
        }

        public void OnSaveValues()
        {
        }

        private void OnModPageClick(object sender, RoutedEventArgs e)
        {
            Utils.OpenUrl("https://www.gta5-mods.com/scripts/chaos-mod-v");
        }

        private void OnPaypalClick(object sender, RoutedEventArgs e)
        {
            Utils.OpenUrl("https://paypal.me/EmrCue");
        }

        private void OnGithubClick(object sender, RoutedEventArgs e)
        {
            Utils.OpenUrl("https://github.com/gta-chaos-mod/ChaosModV");
        }

        private void OnDiscordClick(object sender, RoutedEventArgs e)
        {
            Utils.OpenUrl("https://discord.gg/w2tDeKVaF9");
        }
    }
}
