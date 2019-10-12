using System;
using System.Threading;

namespace ConfigApp
{
    public class EntryPoint
    {
        [STAThread]
        public static void Main(string[] args)
        {
            Mutex mutex = new Mutex(false, "com.pongo1231.chaosmodv");

            if (!mutex.WaitOne(TimeSpan.FromSeconds(5), false))
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
