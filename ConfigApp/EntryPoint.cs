using System.Runtime.InteropServices;
using System.Threading;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using WinRT;

namespace ConfigApp
{
    public class EntryPoint
    {
        [STAThread]
        public static void Main()
        {
            XamlCheckProcessRequirements();
            ComWrappersSupport.InitializeComWrappers();

            Mutex mutex = new(false, "ChaosModVConfigMutex");

            if (!mutex.WaitOne(100))
            {
                return;
            }

            try
            {
                Application.Start((applicationInitializationCallbackParams) =>
                {
                    SynchronizationContext.SetSynchronizationContext(new DispatcherQueueSynchronizationContext(DispatcherQueue.GetForCurrentThread()));
                    var app = new App();
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
