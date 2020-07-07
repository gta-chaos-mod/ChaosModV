import { TMessageListener } from './tMessageListener';

export interface IChaosWSClient {
	addCreateVoteListener(listener: TMessageListener): void;
	addUpdateVoteListener(listener: TMessageListener): void;
	removeCreateVoteListener(listener: TMessageListener): void;
	removeUpdateVoteListener(listener: TMessageListener): void;
}
