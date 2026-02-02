using System;
using System.Threading.Tasks;

namespace TwitchChatVotingProxy.VotingReceiver
{
    public interface IVotingReceiver
    {
        event EventHandler<OnMessageArgs> OnMessage;
        Task<bool> Init();
        Task SendMessage(string msg);
    }
}