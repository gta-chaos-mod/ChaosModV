import { IChaosOverlayVoteOption } from './iVoteOption';

export interface IChaosOverlayClientMessage {
	request: 'CREATE' | 'UPDATE';
	totalVotes: number;
	votingMode: 'MAJORITY' | 'PERCENTAGE';
	voteOptions: IChaosOverlayVoteOption[];
}
