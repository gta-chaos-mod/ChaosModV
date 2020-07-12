export interface ILiteEvent<T> {
	addEventListener(handler: { (data: T): void }): void;
	removeEventListener(handler: { (data: T): void }): void;
}
