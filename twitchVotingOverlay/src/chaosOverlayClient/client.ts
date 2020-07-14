import { IChaosOverlayClient } from './iClient';
import { LiteEvent } from '../lightEvent';
import { IChaosOverlayVoteOption } from './iVoteOption';
import { IChaosOverlayClientMessage } from './iMessage';
import { TChaosOverlayClientEvent } from './tEvent';

export class ChaosOverlayClient implements IChaosOverlayClient {
	/**
	 * Interval in which the websocket client tries to reconnect to
	 * the server
	 */
	public static RECONNECT_INTERVAL = 1000;

	private URL: string;
	private WS: WebSocket | null = null;
	private createEvent = new LiteEvent<IChaosOverlayClientMessage>();
	private connectEvent = new LiteEvent();
	private disconnectEvent = new LiteEvent();
	private endEvent = new LiteEvent<IChaosOverlayClientMessage>();
	private noVoteRoundEvent = new LiteEvent<IChaosOverlayClientMessage>();
	private updateEvent = new LiteEvent<IChaosOverlayClientMessage>();

	public constructor(URL: string) {
		this.URL = URL;

		// Bind the listener methods
		this.onSocketClose = this.onSocketClose.bind(this);
		this.onSocketError = this.onSocketError.bind(this);
		this.onSocketMessage = this.onSocketMessage.bind(this);
		this.onSocketOpen = this.onSocketOpen.bind(this);

		this.connect();
	}

	public addCreateVoteListener(listener: TChaosOverlayClientEvent): void {
		this.createEvent.addEventListener(listener);
	}
	public addConnectListener(listener: () => void): void {
		this.connectEvent.addEventListener(listener);
	}
	public addDisconnectListener(listener: () => void): void {
		this.disconnectEvent.addEventListener(listener);
	}
	public addEndVoteListener(listener: TChaosOverlayClientEvent): void {
		this.endEvent.addEventListener(listener);
	}
	public addNoVotingRoundListener(listener: TChaosOverlayClientEvent): void {
		this.noVoteRoundEvent.addEventListener(listener);
	}
	public addUpdateVoteListener(listener: TChaosOverlayClientEvent): void {
		this.updateEvent.addEventListener(listener);
	}
	public removeCreateVoteListener(listener: TChaosOverlayClientEvent): void {
		this.createEvent.removeEventListener(listener);
	}
	public removeConnectListener(listener: () => void): void {
		this.connectEvent.removeEventListener(listener);
	}
	public removeOnDisconnectListener(listener: () => void): void {
		this.disconnectEvent.removeEventListener(listener);
	}
	public removeEndVoteListener(listener: TChaosOverlayClientEvent): void {
		this.endEvent.removeEventListener(listener);
	}
	public removeNoVotingRoundListener(listener: TChaosOverlayClientEvent): void {
		this.noVoteRoundEvent.removeEventListener(listener);
	}
	public removeUpdateVoteListener(listener: TChaosOverlayClientEvent): void {
		this.updateEvent.removeEventListener(listener);
	}

	/**
	 * Connects to the websocket server and adds event listeners
	 */
	private connect(): void {
		try {
			this.WS = new WebSocket(this.URL);
			this.WS.addEventListener('close', this.onSocketClose);
			// "as any" is required because typescript thinks that the websocket "onError"
			// has an event, and not and error as callback data
			this.WS.addEventListener('error', this.onSocketError as any);
			this.WS.addEventListener('message', this.onSocketMessage);
			this.WS.addEventListener('open', this.onSocketOpen);
		} catch (e) {
			// If the websocket connection fails we get an error,
			// this is caught by the "onError" callback
		}
	}
	/**
	 * Callback for the websocket client - called on errors
	 * @param error what went wrong
	 */
	private onSocketError(error: Error): void {
		console.log(`error in socket occurred: ${error.message}. closing socket`);
		if (this.WS !== null) this.WS.close();
	}
	/**
	 * Callback for the websocket client - called on socket close
	 */
	private onSocketClose(): void {
		console.log(`socket closed, reconnecting in ${ChaosOverlayClient.RECONNECT_INTERVAL}ms`);
		this.disconnectEvent.dispatch(null);
		window.setTimeout(() => this.connect(), ChaosOverlayClient.RECONNECT_INTERVAL);
	}
	/**
	 * Callback for the websocket client - called when a message is received
	 * @param message message that was received
	 */
	private onSocketMessage(message: MessageEvent): void {
		try {
			const MESSAGE: IChaosOverlayClientMessage = JSON.parse(message.data);

			switch (MESSAGE.request) {
				case 'CREATE':
					this.createEvent.dispatch(MESSAGE);
					break;
				case 'END':
					this.endEvent.dispatch(MESSAGE);
					break;
				case 'NO_VOTING_ROUND':
					this.noVoteRoundEvent.dispatch(MESSAGE);
					break;
				case 'UPDATE':
					this.updateEvent.dispatch(MESSAGE);
					break;
				default:
					console.warn(`unknown request type: ${MESSAGE.request}`);
			}
		} catch (e) {
			console.error(`failed to parse json data: ${e}`);
		}
	}
	/**
	 * Callback for the websocket client - called on socket open
	 */
	private onSocketOpen(): void {
		console.log('successfully connected to websocket');
		this.connectEvent.dispatch(null);
	}
}
