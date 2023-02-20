#include <stdafx.h>

#ifdef WITH_DEBUG_PANEL_SUPPORT
#include "DebugSocket.h"

#include "Components/EffectDispatcher.h"
#include "Effects/EnabledEffectsMap.h"
#include "Util/OptionsFile.h"

#include <json.hpp>

#define LISTEN_PORT 31819

using nlohmann::json;

static void QueueDelegate(DebugSocket *debugSocket, std::function<void()> delegate)
{
	std::lock_guard lock(debugSocket->m_DelegateQueueMutex);
	debugSocket->m_DelegateQueue.push(delegate);
}

static void OnFetchEffects(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                           ix::WebSocket &webSocket, const json &payloadJson)
{
	json effectsJson;
	for (const auto &[effectId, effectData] : g_dictEnabledEffects)
	{
		if (effectData.TimedType == EEffectTimedType::Permanent || effectData.IsHidden())
		{
			continue;
		}

		json effectInfoJson;
		effectInfoJson["id"]   = effectId.GetEffectId();
		effectInfoJson["name"] = effectData.Name;

		effectsJson["effects"].push_back(effectInfoJson);
	}

	webSocket.send(effectsJson.dump());
}

static void OnTriggerEffect(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                            ix::WebSocket &webSocket, const json &payloadJson)
{
	if (!ComponentExists<EffectDispatcher>())
	{
		return;
	}

	if (!payloadJson.contains("effect_id") || !payloadJson["effect_id"].is_string())
	{
		return;
	}

	auto targetEffectId = payloadJson["effect_id"].get<std::string>();

	auto result         = g_dictEnabledEffects.find(targetEffectId);
	if (result != g_dictEnabledEffects.end())
	{
		QueueDelegate(debugSocket, [result]() { GetComponent<EffectDispatcher>()->DispatchEffect(result->first); });
	}
}

static void OnExecScript(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                         ix::WebSocket &webSocket, const json &payloadJson)
{
	if (!ComponentExists<EffectDispatcher>())
	{
		return;
	}

	if (!payloadJson.contains("script_raw") || !payloadJson["script_raw"].is_string())
	{
		return;
	}

	QueueDelegate(debugSocket,
	              [payloadJson]()
	              {
		              // Generate random hex value for script name
		              std::string scriptName;
		              scriptName.resize(8);
		              for (int i = 0; i < 8; i++)
		              {
			              sprintf(scriptName.data() + i, "%x", g_Random.GetRandomInt(0, 16));
		              }

		              LuaScripts::RegisterScriptRawTemporary(scriptName, payloadJson["script_raw"]);
	              });
}

static void OnMessage(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                      ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg)
{
	if (msg->type != ix::WebSocketMessageType::Message)
	{
		return;
	}

	auto payload = msg->str;

	json payloadJson;
	try
	{
		payloadJson = json::parse(payload);
	}
	catch (json::parse_error &exception)
	{
		LOG("Received invalid message: " << exception.what());
		return;
	}

	if (!payloadJson.contains("command"))
	{
		return;
	}

	auto command = payloadJson["command"].get<std::string>();
#define SET_HANDLER(cmd, handler) \
	if (command == cmd)           \
		handler(debugSocket, connectionState, webSocket, payloadJson);

	SET_HANDLER("fetch_effects", OnFetchEffects);
	SET_HANDLER("trigger_effect", OnTriggerEffect);
	SET_HANDLER("exec_script", OnExecScript);

#undef HANDLER
}

DebugSocket::DebugSocket()
{
	m_Server = std::make_unique<ix::WebSocketServer>(LISTEN_PORT, "127.0.0.1");

	m_Server->setOnClientMessageCallback([&](std::shared_ptr<ix::ConnectionState> connectionState,
	                                         ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg)
	                                     { OnMessage(this, connectionState, webSocket, msg); });
	m_Server->listenAndStart();

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
	LOG("Listening for incoming connections on port " STR(LISTEN_PORT));
#undef STR_HELPER
#undef STR
}

DebugSocket::~DebugSocket()
{
	Close();
}

void DebugSocket::Close()
{
	m_Server->stop();
}

void DebugSocket::OnModPauseCleanup()
{
	Close();
}

void DebugSocket::OnRun()
{
	if (!m_DelegateQueue.empty())
	{
		std::lock_guard lock(m_DelegateQueueMutex);
		while (!m_DelegateQueue.empty())
		{
			m_DelegateQueue.front()();
			m_DelegateQueue.pop();
		}
	}
}

#endif