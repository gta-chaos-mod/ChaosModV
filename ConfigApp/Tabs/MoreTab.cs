using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace ConfigApp.Tabs
{
    public class MoreTab : Tab
    {
        private static Button GenerateInfoButton(string text, RoutedEventHandler eventHandler)
        {
            var button = new Button
            {
                Content = text,
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

            var rows = new (string Text, string ButtonText, string Url)[]
            {
                ("Visit the mod page for more information", "GTA5-Mods", "https://www.gta5-mods.com/scripts/chaos-mod-v"),
                ("Enjoying the mod? Buy me a coffee! :)", "PayPal", "https://paypal.me/EmrCue"),
                ("Want to contribute to the mod? It's open source!", "GitHub", "https://github.com/gta-chaos-mod/ChaosModV"),
                ("Join our Discord for announcements, support and more!", "Discord", "https://discord.gg/w2tDeKVaF9")
            };

            foreach (var row in rows)
            {
                PushRowPair(row.Text, GenerateInfoButton(row.ButtonText, (_, _) => Utils.OpenURL(row.Url)));
                PopRow();
            }
        }
    }
}
