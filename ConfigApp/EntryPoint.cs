using System.Runtime.InteropServices;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using WinRT;

namespace ConfigApp
{
    public sealed class EntryPoint
    {
        [STAThread]
        public static void Main()
        {
            XamlCheckProcessRequirements();
            ComWrappersSupport.InitializeComWrappers();

            Mutex mutex = new(false, "ChaosModVConfigMutex");

            if (!mutex.WaitOne(100))
                return;

            try
            {
                Application.Start(_ =>
                {
                    SynchronizationContext.SetSynchronizationContext(new DispatcherQueueSynchronizationContext(DispatcherQueue.GetForCurrentThread()));
                    App app = new App();
                });
            }
            finally
            {
                mutex.ReleaseMutex();
            }
        }

        [DllImport("Microsoft.ui.xaml.dll")]
        private static extern void XamlCheckProcessRequirements();
    }
}
