using ConfigApp.Infrastructure;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

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

            if (MainWindow.Content is FrameworkElement rootElement)
                AppDialog.Initialize(rootElement.XamlRoot);

            base.OnLaunched(args);
        }
    }
}
