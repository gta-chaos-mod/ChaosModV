﻿using System.Collections.Generic;

namespace ChaosPipe
{
    public interface IVoteOption
    {
        /// <summary>
        /// Label (name) of the voting option
        /// </summary>
        string Label { get; set; }
        /// <summary>
        /// What strings should be counted as voting for this option (e.g. 1, 5, etc)
        /// </summary>
        List<string> Matches { get; }
        /// <summary>
        /// How many votes this voting option has
        /// </summary>
        int Votes { get; set; }
    }
}
