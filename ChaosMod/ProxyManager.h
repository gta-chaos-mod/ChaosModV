#pragma once

#include "EffectDispatcher.h"

#include "ProxyBase.h"

#include <vector>
#include <memory>

class ProxyManager : public ProxyBase
{
public:
	ProxyManager();
	void Tick();

protected:
	virtual bool OnMessageUnhandled(const std::string& msg);
};

