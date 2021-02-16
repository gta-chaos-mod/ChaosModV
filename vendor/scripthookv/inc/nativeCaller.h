/*
	THIS FILE IS A PART OF GTA V SCRIPT HOOK SDK
				http://dev-c.com
			(C) Alexander Blade 2015
*/

#pragma once

#pragma warning( disable : 4789 )

#include "main.h"

// Zorg93
template <typename T>
static inline void nativePush(const T& value)
{
	UINT64 val64 = 0;
	*reinterpret_cast<T*>(&val64) = value;
	nativePush64(val64);
}

template <typename T, typename... TArgs>
static inline void nativePush(const T& value, const TArgs& ... args)
{
	nativePush(value);
	nativePush(args...);
}

template <typename R, typename... TArgs>
static inline R invoke(UINT64 hash, const TArgs& ... args)
{
	nativeInit(hash);
	nativePush(args...);
	return *reinterpret_cast<R*>(nativeCall());
}

template <typename R>
inline R invoke(UINT64 hash)
{
	nativeInit(hash);
	return *reinterpret_cast<R*>(nativeCall());
}