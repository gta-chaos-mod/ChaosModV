#include <stdafx.h>

#ifdef WITH_DEBUG_PANEL_SUPPORT

#include "DebugSocket.h"

#include "Components/EffectDispatcher.h"
#include "Effects/EnabledEffects.h"

#include <json.hpp>

#define LISTEN_PORT 31819

#define TRACING_ENTRIES_HISTORY_SECONDS 10

using nlohmann::json;

static void QueueDelegate(DebugSocket *debugSocket, std::function<void()> delegate)
{
	std::lock_guard lock(debugSocket->m_DelegateQueueMutex);
	debugSocket->m_DelegateQueue.push(delegate);
}

static void OnFetchEffects(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                           ix::WebSocket &webSocket, const json &payloadJson)
{
	// TODO: This isn't thread safe currently!

	json effectsJson;
	effectsJson["command"] = "result_fetch_effects";
	for (const auto &[effectId, effectData] : g_EnabledEffects)
	{
		if (effectData.TimedType == EffectTimedType::Permanent || effectData.IsHidden())
			continue;

		json effectInfoJson;
		effectInfoJson["id"]   = effectId;
		effectInfoJson["name"] = effectData.Name;

		effectsJson["effects"].push_back(effectInfoJson);
	}

	webSocket.send(effectsJson.dump());
}

static void OnTriggerEffect(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                            ix::WebSocket &webSocket, const json &payloadJson)
{
	if (!ComponentExists<EffectDispatcher>())
		return;

	if (!payloadJson.contains("effect_id") || !payloadJson["effect_id"].is_string())
		return;

	auto targetEffectId = payloadJson["effect_id"].get<std::string>();
	if (targetEffectId.empty())
		return;

	QueueDelegate(debugSocket,
	              [targetEffectId]()
	              {
		              auto result = g_EnabledEffects.find(targetEffectId);
		              if (result != g_EnabledEffects.end())
			              GetComponent<EffectDispatcher>()->DispatchEffect(result->first);
	              });
}

static void OnExecScript(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                         ix::WebSocket &webSocket, const json &payloadJson)
{
	if (!ComponentExists<EffectDispatcher>())
		return;

	if (!payloadJson.contains("script_raw") || !payloadJson["script_raw"].is_string())
		return;

	auto script = payloadJson["script_raw"].get<std::string>();
	if (script.empty())
		return;

	// Generate random hex value for script name
	std::string scriptName;
	scriptName.resize(8);
	for (int i = 0; i < 8; i++)
		sprintf(scriptName.data() + i, "%x", g_RandomNoDeterm.GetRandomInt(0, 16));

	json json;
	json["command"]     = "result_exec_script";
	json["script_name"] = scriptName;
	webSocket.send(json.dump());

	QueueDelegate(debugSocket,
	              [payloadJson, scriptName]()
	              {
		              if (ComponentExists<LuaScripts>())
			              GetComponent<LuaScripts>()->RegisterScriptRawTemporary(scriptName, payloadJson["script_raw"]);
	              });
}

static void OnSetProfileState(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                              ix::WebSocket &webSocket, const json &payloadJson)
{
	if (!payloadJson.contains("state") || !payloadJson["state"].is_string())
		return;

	const auto &state = payloadJson["state"];
	if (state == "start")
	{
		if (!debugSocket->m_IsProfiling)
		{
			debugSocket->m_IsProfiling = true;
			QueueDelegate(debugSocket, []() { LOG("Started Profiling!"); });
		}
	}
	else if (state == "stop")
	{
		if (debugSocket->m_IsProfiling)
		{
			debugSocket->m_IsProfiling = false;
			QueueDelegate(debugSocket,
			              [debugSocket]()
			              {
				              debugSocket->m_EffectTraceStats.clear();
				              LOG("Stopped Profiling!");
			              });
		}
	}
	else if (state == "fetch")
	{
		if (debugSocket->m_IsProfiling)
		{
			QueueDelegate(debugSocket,
			              [debugSocket, &webSocket]()
			              {
				              json resultJson;
				              resultJson["command"]  = "profile_state";
				              resultJson["profiles"] = json({});
				              for (const auto &[effectId, traceStats] : debugSocket->m_EffectTraceStats)
				              {
					              if (traceStats.TotalExecTime == 0)
						              continue;

					              json profileJson;
					              profileJson["total_exec_time"] = traceStats.TotalExecTime;
					              profileJson["max_exec_time"]   = traceStats.MaxExecTime;

					              for (const auto &execTrace : traceStats.ExecTraces)
						              profileJson["exec_times"].push_back(execTrace.ExecTime);

					              resultJson["profiles"][effectId] = profileJson;
				              }

				              webSocket.send(resultJson.dump());
			              });
		}
	}
}

static void OnMessage(DebugSocket *debugSocket, std::shared_ptr<ix::ConnectionState> connectionState,
                      ix::WebSocket &webSocket, const ix::WebSocketMessagePtr &msg)
{
	if (msg->type != ix::WebSocketMessageType::Message)
		return;

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
		return;

	auto command = payloadJson["command"].get<std::string>();
#define SET_HANDLER(cmd, handler) \
	if (command == cmd)           \
		handler(debugSocket, connectionState, webSocket, payloadJson);

	SET_HANDLER("fetch_effects", OnFetchEffects);
	SET_HANDLER("trigger_effect", OnTriggerEffect);
	SET_HANDLER("exec_script", OnExecScript);
	SET_HANDLER("profile_state", OnSetProfileState);

#undef HANDLER
}

static bool EventOnPreDispatchEffect(DebugSocket *debugSocket, const EffectIdentifier &ide)
{
	debugSocket->m_EffectTraceStats.erase(ide);
	return true;
}

static void EventOnPreRunEffect(DebugSocket *debugSocket, const EffectIdentifier &id)
{
	if (debugSocket->m_IsProfiling)
	{
		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);

		debugSocket->m_EffectTraceStats[id].EntryTimestamp = ticks.QuadPart;
	}
}

static void EventOnPostRunEffect(DebugSocket *debugSocket, const EffectIdentifier &id)
{
	if (!debugSocket->m_IsProfiling)
		return;

	const auto &effectId = id;
	if (!debugSocket->m_EffectTraceStats.contains(effectId))
		return;

	auto &traceStats = debugSocket->m_EffectTraceStats.at(effectId);

	LARGE_INTEGER ticks;
	QueryPerformanceCounter(&ticks);
	std::uint64_t timestamp = ticks.QuadPart;

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	auto execTime = (timestamp - traceStats.EntryTimestamp) / static_cast<float>(freq.QuadPart);
	traceStats.TotalExecTime += execTime;

	traceStats.ExecTraces.push_back({ .Timestamp = timestamp, .ExecTime = execTime });
	if ((timestamp - traceStats.ExecTraces.front().Timestamp) / static_cast<float>(freq.QuadPart)
	    > TRACING_ENTRIES_HISTORY_SECONDS)
	{
		traceStats.ExecTraces.pop_front();
	}

	if (execTime > traceStats.MaxExecTime)
		traceStats.MaxExecTime = execTime;
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

	if (ComponentExists<EffectDispatcher>())
	{
		m_OnPreDispatchEffectListener.Register(GetComponent<EffectDispatcher>()->OnPreDispatchEffect,
		                                       [&](const EffectIdentifier &id)
		                                       { return EventOnPreDispatchEffect(this, id); });

		m_OnPreRunEffectListener.Register(GetComponent<EffectDispatcher>()->OnPreRunEffect,
		                                  [&](const EffectIdentifier &id) { EventOnPreRunEffect(this, id); });

		m_OnPostRunEffectListener.Register(GetComponent<EffectDispatcher>()->OnPostRunEffect,
		                                   [&](const EffectIdentifier &id) { EventOnPostRunEffect(this, id); });
	}
}

void DebugSocket::Close()
{
	m_Server->stop();
}

void DebugSocket::ScriptLog(std::string_view scriptName, std::string_view text)
{
	if (!m_Server)
		return;

	json json;
	json["command"]     = "script_log";
	json["script_name"] = scriptName;
	json["text"]        = text;

	for (auto client : m_Server->getClients())
		client->send(json.dump());
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