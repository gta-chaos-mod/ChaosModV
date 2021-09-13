using System.Text;
using System.Threading.Tasks;
using System.Net;


namespace ConfigApp
{
    class HTTPServer
    {
        private static HttpListener listener;
        private static string url = "http://localhost:8876/";
        private bool running = false;

        private MainWindow m_Main;

        public HTTPServer(MainWindow main)
        {
            listener = new HttpListener();
            listener.Prefixes.Add(url);
            listener.Start();
            running = true;

            m_Main = main;

            HandleIncomingConnections();
        }

        private async Task HandleIncomingConnections()
        {
            while (running)
            {
                HttpListenerContext ctx = await listener.GetContextAsync();

                HttpListenerRequest req = ctx.Request;
                HttpListenerResponse res = ctx.Response;

                byte[] data = Encoding.UTF8.GetBytes("Not found.");
                res.ContentType = "text/plain";
                res.ContentEncoding = Encoding.UTF8;

                if (req.Url.AbsolutePath == "/oauth/callback") {
                    string token = req.QueryString.Get("code");
                    m_Main.SetOauthToken(token);
                    data = Encoding.UTF8.GetBytes("All done! Return to the Config App");
                }

                res.ContentLength64 = data.LongLength;

                await res.OutputStream.WriteAsync(data, 0, data.Length);
                res.Close();
            }
        }

        public void Stop()
        {
            running = false;
            listener.Stop();
        }
    }
}
