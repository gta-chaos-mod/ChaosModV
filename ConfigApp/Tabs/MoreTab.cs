using System.Windows;
using System.Windows.Controls;

namespace ConfigApp.Tabs
{
    public class MoreTab : Tab
    {
        private Button GenerateInfoButton(string text, RoutedEventHandler eventHandler)
        {
            var button = new Button()
            {
                Content = text,
                Width = 75f,
                Height = 20f,
                HorizontalAlignment = HorizontalAlignment.Right,
                VerticalAlignment = VerticalAlignment.Center
            };
            button.Click += eventHandler;

            return button;
        }

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(310f));
            PushNewColumn(new GridLength(85f));

            PushRowPair("Visit the mod page for more information", GenerateInfoButton("GTA5-Mods", (sender, eventArgs) =>
            {
                System.Diagnostics.Process.Start("https://www.gta5-mods.com/scripts/chaos-mod-v-beta");
            }));
            PopRow();

            PushRowPair("Enjoying the mod? Buy me a coffee! :)", GenerateInfoButton("PayPal", (sender, eventArgs) =>
            {
                System.Diagnostics.Process.Start("https://paypal.me/EmrCue");
            }));
            PopRow();

            PushRowPair("Want to contribute to the mod? It's open source!", GenerateInfoButton("GitHub", (sender, eventArgs) =>
            {
                System.Diagnostics.Process.Start("https://github.com/gta-chaos-mod/ChaosModV");
            }));
            PopRow();

            PushRowPair("Join our Discord for announcements, support and more!", GenerateInfoButton("Discord", (sender, eventArgs) =>
            {
                System.Diagnostics.Process.Start("https://discord.gg/w2tDeKVaF9");
            }));
        }
    }
}
