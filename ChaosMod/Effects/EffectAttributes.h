#pragma once

enum class EffectAttributes
{
	ExcludedFromVoting  = (1 << 0),
	IsMeta              = (1 << 1),
	IsUtility           = (1 << 2),
	IsTemporary         = (1 << 3),
	HideRealNameOnStart = (1 << 4)
};

inline EffectAttributes operator|(EffectAttributes lhs, EffectAttributes rhs)
{
	return static_cast<EffectAttributes>(static_cast<std::underlying_type<EffectAttributes>::type>(lhs)
	                                     | static_cast<std::underlying_type<EffectAttributes>::type>(rhs));
}

inline void operator|=(EffectAttributes &lhs, EffectAttributes rhs)
{
	lhs = lhs | rhs;
}

inline EffectAttributes operator&(EffectAttributes lhs, EffectAttributes rhs)
{
	return static_cast<EffectAttributes>(static_cast<std::underlying_type<EffectAttributes>::type>(lhs)
	                                     & static_cast<std::underlying_type<EffectAttributes>::type>(rhs));
}

inline void operator&=(EffectAttributes &lhs, EffectAttributes rhs)
{
	lhs = lhs & rhs;
}

inline EffectAttributes operator~(EffectAttributes lhs)
{
	return static_cast<EffectAttributes>(~static_cast<std::underlying_type<EffectAttributes>::type>(lhs));
}