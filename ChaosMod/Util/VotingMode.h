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

	std::string ToString()
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
	}

  private:
	Value value;
};