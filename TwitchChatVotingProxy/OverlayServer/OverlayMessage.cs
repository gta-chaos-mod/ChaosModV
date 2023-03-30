using System.Windows.Media;

namespace TwitchChatVotingProxy.OverlayServer
{
    enum eMessageType : int
    {
        NONE = -1,
        SET_VOTES,
        SET_COLOR
    }

    /// <summary>
    /// Universal container for voting overlay message data
    /// </summary>
    class OverlayBaseMessage
    {
        /// <summary>
        /// The type of the message request. This is read by the overlay to properly parse the correct request message.
        /// </summary>
        public string type { get; set; }
        /// <summary>
        /// The json data (serialized) of the actual request
        /// </summary>
        public string messageData { get; set; }
    }

    /// <summary>
    /// Message containing voting data
    /// </summary>
    class OverlayVotingMessage
    {
        public bool retainInitialVotes { get; set; }
        /// <summary>
        /// request type
        /// </summary>
        public string request { get; set; }
        /// <summary>
        /// total vote count
        /// </summary>
        public int totalVotes { get; set; }
        /// <summary>
        /// what voting mode should be used, this results in display changes
        /// </summary>
        public string votingMode { get; set; }
        /// <summary>
        /// Voting options them self
        /// </summary>
        public OverlayVoteOption[] voteOptions { get; set; }
    }

    /// <summary>
    /// Message to tell the voting overlay to update the style color for the bar progression
    /// </summary>
    class OverlayColorMessage
    {
        /// <summary>
        /// Red value of the style color
        /// </summary>
        public int colorR;
        /// <summary>
        /// Green value of the style color
        /// </summary>
        public int colorG;
        /// <summary>
        /// Blue value of the style color
        /// </summary>
        public int colorB;
    }
}
