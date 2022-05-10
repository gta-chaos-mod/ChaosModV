using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Windows.Resources;

namespace ChaosModInstaller
{
    /// <summary>
    /// Interaction logic for Page1.xaml
    /// </summary>
    public partial class StartUp : Page
    {

        public StartUp()
        {
            InitializeComponent();
        }

        public void start_gta_btn_click(object sender, EventArgs e)
        {
            SetupPage page = new SetupPage(InstallationType.GTA5);
            this.NavigationService.Navigate(page);
        }

        public void start_fivem_btn_click(object sender, EventArgs e)
        {
            SetupPage page = new SetupPage(InstallationType.FIVEM);
            this.NavigationService.Navigate(page);
        }
    }
}
