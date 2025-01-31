#pragma once

#include <string>

class VotingMode
{
  public:
	enum Value
	{
		None,
		Majority,
		Percentage,
		Antimajority
	};

	VotingMode() : value(None)
	{
	}

	VotingMode(Value mode) : value(mode)
	{
	}

	bool operator==(VotingMode a) const
	{
		return value == a.value;
	}
	bool operator!=(VotingMode a) const
	{
		return value != a.value;
	}

	std::string_view ToString() const
	{
		switch (value)
		{
		case VotingMode::None:
			return "NONE";
		case VotingMode::Majority:
			return "MAJORITY";
		case VotingMode::Percentage:
			return "PERCENTAGE";
		case VotingMode::Antimajority:
			return "ANTIMAJORITY";
		}

		return "UNKNOWN";
	}

  private:
	Value value;
};