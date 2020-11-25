using System.Windows;
using System.Windows.Input;
using System.Windows.Media;
using Shared;
using System.IO;
using System;

namespace TwitchOverlayConfig
{
    public partial class MainWindow : Window
    {
        public static OptionsFile m_overlayConfig = new OptionsFile("overlay.ini");

        public MainWindow()
        {
            Init();
        }

        public void Init()
        {
            InitializeComponent();

            // Warning popup
            MessageBox.Show("This is an experimental third-party config utility for editing the look of the OBS overlay. Please report any issues you encounter on the GitHub page!", "Notice", MessageBoxButton.OK, MessageBoxImage.Warning);

            ParseOverlayConfig();
        }

        private void ParseOverlayConfig()
        {
            m_overlayConfig.ReadFile();

            // Basic settings
            overlay_global_bold_text.IsChecked = m_overlayConfig.ReadValueBool("GlobalBoldText", true);
            overlay_global_outline_thickness.Text = m_overlayConfig.ReadValue("GlobalOutlineThickness", "0.6");
            overlay_global_text_size.Text = m_overlayConfig.ReadValue("GlobalTextSize", "16");

            overlay_bar_height.Text = m_overlayConfig.ReadValue("BarHeight", "30");
            overlay_bar_width.Text = m_overlayConfig.ReadValue("BarWidth", "400");

            // Setting the diffrent colors
            if(m_overlayConfig.HasKey("GlobalTextColor"))
            {
                overlay_global_text_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("GlobalTextColor"));
            }
            if(m_overlayConfig.HasKey("GlobalTextOutlineColor"))
            {
                overlay_global_outline_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("GlobalTextOutlineColor"));
            }
            if(m_overlayConfig.HasKey("BarProgressionColor"))
            {
                overlay_bar_progression_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("BarProgressionColor"));
            }
            if(m_overlayConfig.HasKey("BarBackgroundColor"))
            {
                overlay_bar_background_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("BarBackgroundColor"));
            }
            if(m_overlayConfig.HasKey("BarProgessionColorDisabled"))
            {
                overlay_bar_progression_color_disabled.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("BarProgressionColorDisabled"));
            }
            if(m_overlayConfig.HasKey("BarBackgroundColorDisabled"))
            {
                overlay_bar_background_color_disabled.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("BarBackgroundColorDisabled"));
            }
            if(m_overlayConfig.HasKey("BarTextColorDisabled"))
            {
                overlay_bar_text_color_disabled.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("BarTextColorDisabled"));
            }

            // Option to use diffrent colors etc. for the Bars (compared to the "total votes text" for example)
            if(m_overlayConfig.ReadValueBool("CustomBarSettings", false))
            {
                custom_bar_settings.IsChecked = true;
                BarUseCustomSettings();
                if(m_overlayConfig.HasKey("BarTextColor"))
                {
                    overlay_bar_text_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("BarTextColor"));
                }
                if(m_overlayConfig.HasKey("BarOutlineColor"))
                {
                    overlay_bar_outline_color.SelectedColor = (Color)ColorConverter.ConvertFromString(m_overlayConfig.ReadValue("BarOutlineColor"));
                }
                overlay_bar_font_size.Text = m_overlayConfig.ReadValue("BarFontSize", "12");
                overlay_bar_bold_text.IsChecked = m_overlayConfig.ReadValueBool("BarBoldText", true);
                overlay_bar_outline_thickness.Text = m_overlayConfig.ReadValue("BarOutlineThickness", "0.25");
            } 
            else
            {
                overlay_bar_text_color.SelectedColor = overlay_global_text_color.SelectedColor;
                overlay_bar_font_size.Text = overlay_global_text_size.Text;
                overlay_bar_bold_text.IsChecked = overlay_global_bold_text.IsChecked;
                overlay_bar_outline_color.SelectedColor = overlay_global_outline_color.SelectedColor;
                overlay_bar_outline_thickness.Text = overlay_global_outline_thickness.Text;
            }
        }

        private void WriteOverlayConfig()
        {
            // Writing all of our Settings into the Config file
            m_overlayConfig.WriteValue("GlobalBoldText", overlay_global_bold_text.IsChecked.Value);
            m_overlayConfig.WriteValue("GlobalOutlineThickness", overlay_global_outline_thickness.Text);
            m_overlayConfig.WriteValue("GlobalTextSize", overlay_global_text_size.Text);
            m_overlayConfig.WriteValue("GlobalTextColor", overlay_global_text_color.SelectedColor.ToString().Remove(1, 2));
            m_overlayConfig.WriteValue("GlobalTextOutlineColor", overlay_global_outline_color.SelectedColor.ToString().Remove(1, 2));
            m_overlayConfig.WriteValue("BarProgressionColor", overlay_bar_progression_color.SelectedColor.ToString().Remove(1, 2));
            m_overlayConfig.WriteValue("BarBackgroundColor", overlay_bar_background_color.SelectedColor.ToString().Remove(1, 2));
            m_overlayConfig.WriteValue("BarProgressionColorDisabled", overlay_bar_progression_color_disabled.SelectedColor.ToString().Remove(1, 2));
            m_overlayConfig.WriteValue("BarBackgroundColorDisabled", overlay_bar_background_color_disabled.SelectedColor.ToString().Remove(1, 2));
            m_overlayConfig.WriteValue("BarTextColorDisabled", overlay_bar_text_color_disabled.SelectedColor.ToString().Remove(1, 2));
            m_overlayConfig.WriteValue("BarWidth", overlay_bar_width.Text);

            m_overlayConfig.WriteFile();

            if(custom_bar_settings.IsChecked.Value)
            {
                m_overlayConfig.WriteValue("CustomBarSettings", true);
                m_overlayConfig.WriteValue("BarTextColor", overlay_bar_text_color.SelectedColor.ToString().Remove(1, 2));
                m_overlayConfig.WriteValue("BarOutlineColor", overlay_bar_outline_color.SelectedColor.ToString().Remove(1, 2));
                m_overlayConfig.WriteValue("BarFontSize", overlay_bar_font_size.Text);
                m_overlayConfig.WriteValue("BarBoldText", overlay_bar_bold_text.IsChecked.Value);
                m_overlayConfig.WriteValue("BarOutlineThickness", overlay_bar_outline_thickness.Text);
            }
        }

        void BarUseCustomSettings()
        {
            bool enabled = custom_bar_settings.IsChecked.GetValueOrDefault();
            // Turn on all buttons related to the custom settings
            overlay_bar_text_color_label.IsEnabled = enabled;
            overlay_bar_text_color.IsEnabled = enabled;
            overlay_bar_font_size_label.IsEnabled = enabled;
            overlay_bar_font_size.IsEnabled = enabled;
            overlay_bar_bold_text_label.IsEnabled = enabled;
            overlay_bar_bold_text.IsEnabled = enabled;
            overlay_bar_outline_color_label.IsEnabled = enabled;
            overlay_bar_outline_color.IsEnabled = enabled;
            overlay_bar_outline_thickness_label.IsEnabled = enabled;
            overlay_bar_outline_thickness.IsEnabled = enabled;
        }

        private void bar_custom_settings_Clicked(object sender, RoutedEventArgs e)
        {
            BarUseCustomSettings();
        }         

        private void OnlyNumbersPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Utils.HandleOnlyNumbersAndPeriodPreviewTextInput(e);
        }

        private void OnlyNumbersAndPeriodPreviewTextInput(object sender, TextCompositionEventArgs e)
        {
            Utils.HandleOnlyNumbersAndPeriodPreviewTextInput(e);
        }

        private void NoSpacePreviewKeyDown(object sender, KeyEventArgs e)
        {
            Utils.HandleNoSpacePreviewKeyDown(e);
        }

        private void user_reset_Click(object sender, RoutedEventArgs e)
        {
            MessageBoxResult result = MessageBox.Show("Do you want to reset your Twitch Overlay Settings?", "Overlay Config",
                MessageBoxButton.YesNo, MessageBoxImage.Question);
            if(result == MessageBoxResult.Yes)
            {
                m_overlayConfig.ResetFile();
                Init();
            }
        }


        
        // I am not aware of a better way to do this. Could have possibly used line breaks but this only takes a few ms to save so It seems fine.
        public static void writeToCss()
        {
            string filePath = @"index.css";
            File.WriteAllText(filePath, String.Empty);

            string bold = "normal";
            if (!m_overlayConfig.ReadValueBool("GlobalBoldText", false))
                bold = "bold";

            string barBold = "normal";
            if (!m_overlayConfig.ReadValueBool("BarBoldText", false))
            {
                bold = "bold";
            }

            string[] lines =
            {
            "*{",
            "margin: 0;",
            "padding: 0;",
            "}",
            "",
            "body{",
            "/* Text */",
            "	--text-color: " +                   m_overlayConfig.ReadValue("GlobalTextColor", "#ffffff") + ";",
            "	--text-font-family: 'Arial'" +";",
            "	--text-font-weight: " + bold + ";",
            "	--text-outline-color: " +           m_overlayConfig.ReadValue("GlobalTextOutlineColor", "#000000") +";",
            "	--text-outline-thickness: " +       m_overlayConfig.ReadValue("GlobalOutlineThickness", "0.6") + "pt;",
            "   --text-size: " +                    m_overlayConfig.ReadValue("GlobalTextSize", "16") + "pt;",
            "",
            "	/* Bar related things */",
            "	--bar-background-disabled: " +      m_overlayConfig.ReadValue("BarBackgroundColorDisabled", "#afafaf") + ";",
            "	--bar-background: " +               m_overlayConfig.ReadValue("BarBackgroundColor", "#727272") + ";",
            "	--bar-font-size: " +               m_overlayConfig.ReadValue("BarFontSize", "12") + ";",
            "	--bar-font-weight: " + barBold + ";",
            "	--bar-font: var(--text-font-family);",
            "	--bar-height: " + m_overlayConfig.ReadValue("BarHeight", "30") + "px;",
            "	--bar-progression-disabled: " +     m_overlayConfig.ReadValue("BarProgessionColorDisabled", "#8d8d8d") + ";",
            "	--bar-progression: " +              m_overlayConfig.ReadValue("BarProgressionColor", "#238beb") + ";",
            "	--bar-text-color-disabled: " +      m_overlayConfig.ReadValue("BarTextColorDisabled", "#f3f3f3") + ";",
            "	--bar-text-color: " +               m_overlayConfig.ReadValue("BarTextColor", "#f1f1f1")+ ";",
            "	--bar-text-outline-color: " +       m_overlayConfig.ReadValue("BarOutlineColor", "#000000") + ";",
            "	--bar-text-outline-thickness: " + m_overlayConfig.ReadValue("BarOutlineThickness", "0.25") + "px;",
            "}",
            "",
            "#barContainer {",
            "   overflow: hidden;",
            "   width: " + m_overlayConfig.ReadValue("BarWidth", "400") + "px;",
            "}",
            "#totalVotes {",
            "   -webkit-text-stroke: var(--text-outline-thickness) var(--text-outline-color);",
            "   color: var(--text-color);",
            "   font-family: var(--text-font-family);",
            "   font-size: var(--text-size);",
            "   font-weight: var(--text-font-weight);",
            "   opacity: 0;",
            "   transition: opacity 200ms ease-in-out;",
            "}",
            "",
            ".bar {",
            "   background-color: var(--bar-background);",
            "   height: var(--bar-height);",
            "   margin: 10px 0;",
            "   position: relative;",
            "   transition: background-color 100ms ease-in-out;",
            "}",
            ".bar.disabled {",
            "   background-color: var(--bar-background);",
            "}",
            ".bar > .labelContainer {",
            "   align-items: center;",
            "   bottom: 0;",
            "   display: flex;",
            "   justify-content: space-between;",
            "   left: 0;",
            "   padding: 0 1em;",
            "   position: absolute;",
            "   right: 0;",
            "   top: 0;",
            "}",
            ".bar > .labelContainer > span {",
            "   -webkit-text-stroke: var(--bar-text-outline-thickness) var(--bar-text-outline-color);",
            "   color: var(--bar-text-color);",
            "   font - family: var(--bar-font);",
            "    font - size: var(--bar-font-size);",
            "    font - weight: var(--bar-font-weight);",
            "transition: color 100ms ease-in-out;",
            "}",

            "",
            ".bar > .labelContainer.disabled > span {",
            "   -webkit-text-stroke: unset;",
            "   color: var(--bar-text-color-disabled);",
            "}",
            ".bar > .progression {",
            "   background-color: var(--bar-progression);",
            "   height: 100%;",
            "   transition: width 0.3s ease-in-out;",
            "}",
            ".bar > .progression.disabled {",
            "   background-color: var(--bar-progression-disabled);",
            "}",
            "",
            ".slideIn {",
                "animation-fill-mode: both;",
                "animation-name: slideIn;",
                "animation-timing-function: ease-in-out;",
            "}",
            ".slideOut {",
                "animation-fill-mode: both;",
                "animation-name: slideOut;",
                "animation-timing-function: ease-in-out;",
            "}",
            "",

            "@keyframes slideIn {",
            "   from {",
            "        transform: translateX(-100%);",
            "     }",
            "        to {",
            "        transform: translateX(0);",
            "    }",
            "}",
            "@keyframes slideOut {",
            "    from {",
            "        transform: translateX(0);",
            "    }",
            "    to {",
            "        transform: translateX(100%);",
            "    }",
            "}",

            };
            
            File.AppendAllLines(filePath, lines);
        }


        private void user_save_Click(object sender, RoutedEventArgs e)
        {
            WriteOverlayConfig();
            writeToCss();
            MessageBox.Show("To see the changes refresh the index.html file in the browser and re-add it to OBS.");
        }
    }
}
