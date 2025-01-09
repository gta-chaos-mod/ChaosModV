namespace ConfigApp
{
    public class EntryPoint
    {
        [STAThread]
        public static void Main()
        {
            Mutex mutex = new(false, "ChaosModVConfigMutex");

            if (!mutex.WaitOne(100))
            {
                return;
            }

            try
            {
                App app = new();
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
