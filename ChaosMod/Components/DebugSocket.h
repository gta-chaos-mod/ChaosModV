#pragma once
#ifdef WITH_DEBUG_PANEL_SUPPORT

#include "Component.h"

#include <ixwebsocket/IXWebSocketServer.h>

#include <functional>
#include <memory>
#include <mutex>
#include <queue>

class DebugSocket : public Component
{
  private:
	std::unique_ptr<ix::WebSocketServer> m_Server;

  public:
	std::queue<std::function<void()>> m_DelegateQueue;
	std::mutex m_DelegateQueueMutex;

	DebugSocket();
	~DebugSocket();

  private:
	void Connect();

  public:
	void Close();

	virtual void OnModPauseCleanup() override;
	virtual void OnRun() override;

	template <class T>
	requires std::is_base_of_v<Component, T>
	friend struct ComponentHolder;
};

#endif