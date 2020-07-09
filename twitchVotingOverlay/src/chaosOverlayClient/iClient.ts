import { TChaosOverlayClientEvent } from './tEvent';

export interface IChaosOverlayClient {
	/**
	 * Adds a listener that is triggered when a new vote is created
	 * @param listener callback that should be called
	 */
	addCreateVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Adds a listener that is triggered when a vote ended
	 * @param listener callback that should be called
	 */
	addEndVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Adds a listener that is triggered when a vote is updated
	 * @param listener callback that should be called
	 */
	addUpdateVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Removes vote creation listener
	 * @param listener callback that should be called
	 */
	removeCreateVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Removes vote end listener
	 * @param listener callback that should be called
	 */
	removeEndVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Removes vote update listener
	 * @param listener callback that should be called
	 */
	removeUpdateVoteListener(listener: TChaosOverlayClientEvent): void;
}
