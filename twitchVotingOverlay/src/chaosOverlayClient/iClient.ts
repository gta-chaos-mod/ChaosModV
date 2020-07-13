import { TChaosOverlayClientEvent } from './tEvent';

export interface IChaosOverlayClient {
	/**
	 * Adds a listener that is triggered when a new vote is created
	 * @param listener callback that should be called
	 */
	addCreateVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Adds a listener that is triggered when the websocket connects
	 * @param listener callback that should be called
	 */
	addConnectListener(listener: () => void): void;
	/**
	 * Adds a listener that is triggered when the websocket disconnects
	 * @param listener callback that should be called
	 */
	addDisconnectListener(listener: () => void): void;
	/**
	 * Adds a listener that is triggered when a vote ended
	 * @param listener callback that should be called
	 */
	addEndVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Adds a listener that is triggered when a vote is updated
	 * @param listener callback that should be called
	 */
	addNoVotingRoundListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Adds a listener that is triggered when a no voting round is created
	 * @param listener callback that should be called
	 */
	addUpdateVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Removes vote creation listener
	 * @param listener callback that should be removed
	 */
	removeCreateVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Removes connect listener
	 * @param listener callback that should be removed
	 */
	removeConnectListener(listener: () => void): void;
	/**
	 * Removes disconnect listener
	 * @param listener callback that should be removed
	 */
	removeOnDisconnectListener(listener: () => void): void;
	/**
	 * Removes vote end listener
	 * @param listener callback that should be removed
	 */
	removeEndVoteListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Removes no voting round listener
	 * @param listener callback that should be removed
	 */
	removeNoVotingRoundListener(listener: TChaosOverlayClientEvent): void;
	/**
	 * Removes vote update listener
	 * @param listener callback that should be removed
	 */
	removeUpdateVoteListener(listener: TChaosOverlayClientEvent): void;
}
