using ConfigApp.Infrastructure;
using Microsoft.UI.Xaml;

namespace ConfigApp
{
    public partial class App : Application
    {
        public static MainWindow? MainWindow { get; private set; }

        public App()
        {
            InitializeComponent();
        }

        protected override void OnLaunched(LaunchActivatedEventArgs args)
        {
            MainWindow = new MainWindow();
            MainWindow.Activate();

            base.OnLaunched(args);
        }
    }
}
