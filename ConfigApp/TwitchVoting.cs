using System.Threading;
using TwitchLib.Client;

namespace ConfigApp
{
    public class TwitchVoting
    {
        private Thread _Thread;

        public TwitchVoting()
        {
            _Thread = new Thread(TwitchLoop);
        }

        public void Init()
        {
            _Thread.Start();
        }

        private void TwitchLoop()
        {
            /*TwitchClient client = new TwitchClient();
            while (true)
            {

            }*/
        }
    }
}
