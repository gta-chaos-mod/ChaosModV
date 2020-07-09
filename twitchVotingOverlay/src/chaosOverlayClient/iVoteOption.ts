export interface IChaosOverlayVoteOption {
	/**
	 * Name for the option
	 */
	label: string;
	/**
	 * Whats the value of this option. This could be
	 * count, a percentage or something entirely different.
	 * It depends on the "voting mode" of the message.
	 */
	value: number;
	/**
	 * What messages should result in voting
	 * for this option.
	 *
	 * An example for this would be: ["1", "a"]
	 */
	matches: string[];
}
