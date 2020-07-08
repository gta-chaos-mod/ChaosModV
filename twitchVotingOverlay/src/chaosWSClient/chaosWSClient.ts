import { IChaosWSClient } from './iChaosWSClient';
import { LiteEvent } from '../lightEvent';
import { IVoteOption } from './iVoteOption';
import { IMessage } from './iMessage';
import { TMessageListener } from './tMessageListener';

export class ChaosWSClient implements IChaosWSClient {
	public static RECONNECT_INTERVAL = 1000;

	private createEvent = new LiteEvent<IMessage>();
	private updateEvent = new LiteEvent<IMessage>();

	private WS: WebSocket | null = null;

	private URL: string;

	public constructor(URL: string) {
		this.URL = URL;

		// Bind the listener methods
		this.onSocketClose = this.onSocketClose.bind(this);
		this.onSocketError = this.onSocketError.bind(this);
		this.onSocketMessage = this.onSocketMessage.bind(this);
		this.onSocketOpen = this.onSocketOpen.bind(this);

		this.connect();
	}

	public addCreateVoteListener(listener: TMessageListener): void {
		this.createEvent.addEventListener(listener);
	}
	public addUpdateVoteListener(listener: TMessageListener): void {
		this.updateEvent.addEventListener(listener);
	}
	public removeCreateVoteListener(listener: TMessageListener): void {
		this.createEvent.removeEventListener(listener);
	}
	public removeUpdateVoteListener(listener: TMessageListener): void {
		this.updateEvent.removeEventListener(listener);
	}

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
	private onSocketError(error: Error): void {
		console.log(`error in socket occurred: ${error.message}. closing socket`);
		if (this.WS !== null) this.WS.close();
	}
	private onSocketClose(): void {
		console.log(`socket closed, reconnecting in ${ChaosWSClient.RECONNECT_INTERVAL}ms`);

		window.setTimeout(() => this.connect(), ChaosWSClient.RECONNECT_INTERVAL);
	}
	private onSocketMessage(message: MessageEvent): void {
		try {
			const MESSAGE: IMessage = JSON.parse(message.data);

			if (MESSAGE.request === 'CREATE') this.createEvent.dispatch(MESSAGE);
			else if (MESSAGE.request === 'UPDATE') this.updateEvent.dispatch(MESSAGE);
			else console.warn(`unknown request type: ${MESSAGE.request}`);
		} catch (e) {
			console.error(`failed to parse json data: ${e}`);
		}
	}
	private onSocketOpen(): void {
		console.log('successfully connected to websocket');
	}
}
