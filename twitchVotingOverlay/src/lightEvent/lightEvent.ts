import { ILiteEvent } from './iLightEvent';

export class LiteEvent<T = null> implements ILiteEvent<T> {
	private handlers: ((data: T) => void)[] = [];

	public addEventListener(handler: (data: T) => void): void {
		this.handlers.push(handler);
	}

	public removeEventListener(handler: (data: T) => void): void {
		this.handlers = this.handlers.filter(h => h !== handler);
	}

	public dispatch(data: T) {
		this.handlers.slice(0).forEach(h => h(data));
	}
}
