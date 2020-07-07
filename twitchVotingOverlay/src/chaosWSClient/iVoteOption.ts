export interface IVoteOption {
	/**
	 * Name for the option
	 */
	label: string;
	/**
	 * How meany votes the option has
	 */
	votes: number;
	/**
	 * What messages should result in voting
	 * for this option.
	 *
	 * An example for this would be: ["1", "a"]
	 */
	matches: string[];
}
