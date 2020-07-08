import { IVoteOption } from './iVoteOption';

export interface IMessage {
	request: 'CREATE' | 'UPDATE';
	totalVotes: number;
	votingMode: 'MAJORITY' | 'PERCENTAGE';
	voteOptions: IVoteOption[];
}
