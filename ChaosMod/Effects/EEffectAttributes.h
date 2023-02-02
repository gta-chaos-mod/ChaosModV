#pragma once

#include <utility>

enum class EEffectAttributes
{
	ExcludedFromVoting = (1 << 0),
	HasCustomName      = (1 << 1),
	IsMeta             = (1 << 2),
	IsUtility          = (1 << 3)
};

inline EEffectAttributes operator|(EEffectAttributes lhs, EEffectAttributes rhs)
{
	return static_cast<EEffectAttributes>(static_cast<std::underlying_type<EEffectAttributes>::type>(lhs)
	                                      | static_cast<std::underlying_type<EEffectAttributes>::type>(rhs));
}

inline void operator|=(EEffectAttributes &lhs, EEffectAttributes rhs)
{
	lhs = lhs | rhs;
}

inline EEffectAttributes operator&(EEffectAttributes lhs, EEffectAttributes rhs)
{
	return static_cast<EEffectAttributes>(static_cast<std::underlying_type<EEffectAttributes>::type>(lhs)
	                                      & static_cast<std::underlying_type<EEffectAttributes>::type>(rhs));
}

inline void operator&=(EEffectAttributes &lhs, EEffectAttributes rhs)
{
	lhs = lhs & rhs;
}

inline EEffectAttributes operator~(EEffectAttributes lhs)
{
	return static_cast<EEffectAttributes>(~static_cast<std::underlying_type<EEffectAttributes>::type>(lhs));
}