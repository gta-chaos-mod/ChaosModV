#pragma once

#include "EffectDispatcher.h"

#include "ProxyBase.h"

#include <vector>
#include <memory>

enum class TwitchOverlayMode
{
	CHAT_MESSAGES,
	OVERLAY_INGAME,
	OVERLAY_OBS
};

class TwitchVoting : public ProxyBase
{
public:
	TwitchVoting();

	void Tick();
	
private:
	TwitchOverlayMode m_twitchOverlayMode;

protected:
	virtual bool OnMessageUnhandled(const std::string& msg);
};