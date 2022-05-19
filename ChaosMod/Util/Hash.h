#pragma once

#include "Natives.h"

using Hash   = unsigned long;
using size_t = unsigned long long;

constexpr Hash operator""_hash(const char *str, size_t n)
{
	return GET_HASH_KEY(str);
}

static_assert("prop_golf_ball"_hash == 0xAF0E3F9F);
static_assert("PROP_GOLF_BALL"_hash == 0xAF0E3F9F);