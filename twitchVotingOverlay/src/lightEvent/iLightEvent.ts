export interface ILiteEvent<T> {
	addEventListener(listener: { (data: T): void }): void;
	removeEventListener(listener: { (data: T): void }): void;
}
