﻿using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;

namespace ConfigApp.Tabs.Voting
{
    public class VotingTab : Tab
    {
        private Dictionary<string, Tab> m_Tabs = new Dictionary<string, Tab>
        {
            { "General", new GeneralTab() },
            { "Twitch", new TwitchTab() }
        };

        protected override void InitContent()
        {
            PushNewColumn(new GridLength(1f, GridUnitType.Star));
            SetRowHeight(new GridLength(1f, GridUnitType.Star));

            var tabControl = new TabControl()
            {
                Background = null,
                BorderBrush = new SolidColorBrush(Color.FromRgb(0xD3, 0xD3, 0xD3)),
                BorderThickness = new Thickness(0f, 1f, 0f, 0f)
            };

            foreach (var tab in m_Tabs)
            {
                var tabItem = new TabItem()
                {
                    Header = tab.Key,
                    Background = new SolidColorBrush(Color.FromRgb(0xF0, 0xF0, 0xF0)),
                    BorderBrush = new SolidColorBrush(Color.FromRgb(0xD3, 0xD3, 0xD3))
                };

                var grid = new Grid();

                tab.Value.Init(grid);

                tabItem.Content = grid;

                tabControl.Items.Add(tabItem);
            }

            PushRowElement(tabControl);
            PopRow();
        }

        public override void OnLoadValues()
        {
            foreach (var tab in m_Tabs)
            {
                tab.Value.OnLoadValues();
            }
        }

        public override void OnSaveValues()
        {
            foreach (var tab in m_Tabs)
            {
                tab.Value.OnSaveValues();
            }
        }
    }
}
