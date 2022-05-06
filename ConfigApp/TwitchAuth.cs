using System;
using System.IO;
using System.Windows;
using System.Net.Http;
using System.Text.RegularExpressions;
using System.Collections.Generic;
using System.Threading.Tasks;

namespace ConfigApp
{
    class TwitchAuth
    {
        private MainWindow m_Main;
        private HTTPServer m_httpServer = null;
        private HttpClient m_httpClient;

        private string m_twitchOauth = "";
        private string m_twitchUsername = "";

        private static string m_twitchLoginUrl = "https://id.twitch.tv/oauth2/authorize?client_id={0}&redirect_uri={1}&response_type=code&scope=chat:read";
        private static string m_twitchRedirectUri = "http://localhost:8876/oauth/callback";
        private string m_twitchClientId = "";
        private string m_twitchClientSecret = "";
        private string m_formattedTwitchLoginUrl = "";

        public TwitchAuth(MainWindow main)
        {
            m_Main = main;
            m_httpClient = new HttpClient();

            if (m_httpServer == null)
            {
                m_httpServer = new HTTPServer(this);
            }

            // DotEnv Handling

            string root = Directory.GetCurrentDirectory();
            string dotenv = Path.Combine(root, ".env");

            if (File.Exists(dotenv))
            {
                DotEnv.Load(dotenv);

                m_twitchClientId = Environment.GetEnvironmentVariable("TWITCH_CLIENT_ID");
                m_twitchClientSecret = Environment.GetEnvironmentVariable("TWITCH_CLIENT_SECRET");
                m_formattedTwitchLoginUrl = string.Format(m_twitchLoginUrl, m_twitchClientId, m_twitchRedirectUri);
            }
        }

        public string OAuthToken
        {
            get => m_twitchOauth;
            set
            {
                m_twitchOauth = value;
            }
        }

        public string Username
        {
            get => m_twitchUsername;
            set
            {
                m_twitchUsername = value;
            }
        }

        public bool LoggedIn
        {
            get => m_twitchOauth == null ? false : m_twitchOauth.Length > 0;
        }

        public void SpawnLogin()
        {
#if DEBUG
            if (m_twitchClientId.Length == 0)
            {
                MessageBox.Show("No TWITCH_CLIENT_ID present. You may be missing your .env file. Skipping login.", "Cannot log in with Twitch", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
            else if (m_twitchClientSecret.Length == 0)
            {
                MessageBox.Show("No TWITCH_CLIENT_SECRET present. You may be missing your .env file. Skipping login.", "Cannot log in with Twitch", MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
#endif

            System.Diagnostics.Process.Start(m_formattedTwitchLoginUrl);
        }

        public async void SetOauthToken(string token)
        {
            m_twitchOauth = "oauth:" + token;

            try
            {
                string code = await GetClientToken(token);
                string username = await GetUsername();
                m_twitchUsername = username;

                m_Main.WriteTwitchFile();
                m_Main.ParseTwitchFile();
            }
            catch { }
        }

        private async Task<string> GetClientToken(string token)
        {
            Dictionary<string, string> values = new Dictionary<string, string>
            {
                {"client_id", m_twitchClientId},
                {"client_secret", m_twitchClientSecret},
                {"code", token},
                {"redirect_uri", m_twitchRedirectUri},
                {"grant_type", "authorization_code"}
            };
            List<string> paramList = new List<string>();
            foreach (KeyValuePair<string, string> parameter in values)
            {
                paramList.Add(string.Format("{0}={1}", parameter.Key, parameter.Value));
            }

            string url = string.Format("https://id.twitch.tv/oauth2/token?{0}", string.Join("&", paramList));
            HttpResponseMessage response = await m_httpClient.PostAsync(url, null);
            string data = await response.Content.ReadAsStringAsync();

            Regex regex = new Regex("{\"access_token\":\"([^\"]*)\"", RegexOptions.IgnoreCase);
            Match match = regex.Match(data);
            if (!match.Success)
            {
                MessageBox.Show("Something went wrong logging you into Twitch.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                throw new Exception("Could not find Twitch access_token");
            }

            string code = match.Groups[1].Value;
            m_httpClient.DefaultRequestHeaders.Add("Authorization", string.Format("Bearer {0}", code));
            m_httpClient.DefaultRequestHeaders.Add("Client-Id", m_twitchClientId);

            return code;
        }

        private async Task<string> GetUsername()
        {
            HttpResponseMessage response = await m_httpClient.GetAsync("https://api.twitch.tv/helix/users");
            string data = await response.Content.ReadAsStringAsync();

            Regex regex = new Regex("\"login\":\"([^\"]*)\"", RegexOptions.IgnoreCase);
            Match match = regex.Match(data);
            if (!match.Success)
            {
                MessageBox.Show("Something went wrong logging you into Twitch.", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                throw new Exception("Could not find Twitch login name");
            }

            string username = match.Groups[1].Value;
            return username;
        }

        public void StopServer()
        {
            if (m_httpServer != null)
            {
                m_httpServer.Stop();
            }
        }
    }
}
