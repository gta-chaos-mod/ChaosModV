using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.IO;
using System.DirectoryServices;
using Microsoft.Win32;
using System.Configuration;

namespace ChaosModInstaller
{

    public enum InstallationType : int
    {
        GTA5,
        FIVEM
    }

    public partial class SetupPage : Page
    {
        private InstallationType installationType;

        private string gtaPath = "";
        private string fivemPath = "";

        public SetupPage(InstallationType type)
        {
            installationType = type;
            InitializeComponent();

            if (installationType == InstallationType.FIVEM)
            {
                directory_browse_fivem.Visibility = Visibility.Visible;
                directory_path_fivem.Visibility = Visibility.Visible;
            }

            directory_path_gta.TextChanged += Gta5TextChanged;
            directory_path_fivem.TextChanged += FivemTextChanged;
        }

        private string BrowseFiles()
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            DialogResult res = fbd.ShowDialog();
            if (res == DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath))
            {
                return fbd.SelectedPath;
            }
            return String.Empty;
        }

        private bool VerifyGtaDirectory(string path)
        {
            if (!File.Exists(path + "\\Gta5.exe"))
            {
                err_msg_gta.Visibility = Visibility.Visible;
                return false;
            }
            else
            {
                err_msg_gta.Visibility = Visibility.Hidden;
                return true;
            }
        }

        private bool VerifyFiveMDirectory(string path)
        {
            if (installationType != InstallationType.FIVEM) return true;
            if (!File.Exists(path + "\\FiveM.exe"))
            {
                err_msg_fivem.Visibility = Visibility.Visible;
                return false;
            }
            else
            {
                err_msg_fivem.Visibility = Visibility.Hidden;
                return true;
            }
        }

        private void Gta5TextChanged(object sender, TextChangedEventArgs e)
        {
            gtaPath = Path.Combine(directory_path_gta.Text, "chaosmod");
            VerifyGtaDirectory(directory_path_gta.Text);
        }

        private void FivemTextChanged(object sender, TextChangedEventArgs e)
        {
            fivemPath = Path.Combine(directory_path_fivem.Text, "FiveM.app\\plugins");
            VerifyFiveMDirectory(directory_path_fivem.Text);
        }

        private void directory_browse_gta_clk(object sender, EventArgs e)
        {
            string s = BrowseFiles();
            directory_path_gta.Text = s;
        }

        private void directory_browse_fivem_clk(object sender, EventArgs e)
        {
            string s = BrowseFiles();
            directory_path_fivem.Text = s;
        }

        private void install_btn_click(object sender, EventArgs e)
        {
            bool b = VerifyFiveMDirectory(directory_path_fivem.Text) && VerifyGtaDirectory(directory_path_gta.Text);
            if (b)
            {
                Install page;
                if (installationType == InstallationType.GTA5)
                {
                    page = new Install(gtaPath, directory_path_gta.Text);
                }
                else
                {
                    page = new Install(gtaPath, fivemPath);
                }
                
                this.NavigationService.Navigate(page);
            }
        }
    }
}
