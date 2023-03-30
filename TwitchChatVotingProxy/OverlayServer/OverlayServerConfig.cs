using System.Windows.Media;

namespace TwitchChatVotingProxy.OverlayServer
{
    /// <summary>
    /// Config for the default implementation of the overlay server
    /// </summary>
    class OverlayServerConfig
    {
        public bool RetainInitialVotes { get; set; }
        public EVotingMode VotingMode { get; set; }
        public int Port { get; set; }
        public Color OverlayVotingBarColor { get; set; }

        public OverlayServerConfig(EVotingMode votingMode, bool retainInitialVotes, int? port, Color overlayVotingBarColor)
        {
            RetainInitialVotes = retainInitialVotes;
            VotingMode = votingMode;
            Port = port == null ? 9091 : (int)port;
            OverlayVotingBarColor = overlayVotingBarColor;
        }
    }
}
