using System;
using System.Threading;

namespace ConfigApp
{
    public class EntryPoint
    {
        [STAThread]
        public static void Main(string[] args)
        {
            Mutex mutex = new Mutex(false, "ChaosModVConfigMutex");

            if (!mutex.WaitOne(100))
            {
                return;
            }

            try
            {
                App app = new App();
                app.InitializeComponent();
                app.Run();
            }
            finally
            {
                mutex.ReleaseMutex();
            }
        }
    }
}
