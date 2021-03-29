# Chaos Mod V

The original chaos mod for GTA V only supports twitch chat as a voting option for others.
This version has only minor changes to the TwitchChatVotingProxy module of the mod.

## What is changed?

Twitch support is completely disabled, but others can vote via a website. Everybody with your uniqu link is then able to vote in their browser.

They can either vote by clicking the action or by entering the number, like otherwise in the twich chat.

## Enable the web functionality

1. Download the [latest release](https://github.com/CaveFire/ChaosModV/releases) and copy the contents to your gta installation folder.
2. Open the app "ChaosModVConfig.exe" (as an administrator) go to the "Twitch" tab.
3. Check the "Enable Twitch Voting" checkbox.
4. Enter something in the textboxes "Channel Name", "Username" and "OAuth Token". It doesn't matter whats in there, as long as there is something in there.
5. Select "OBS Overlay" as your prefered voting overlay mode.
6. Hit "Save"

Start GTA and load a savegame and wait until the mod enabled itself. Then run the "WebChaosMod.exe". You will then get an url which other people can use to connect to you.

## Security

Nobody is able to see the ip address of other participants or from the player, since everything is proxied. There should be no risk in using this mod.
