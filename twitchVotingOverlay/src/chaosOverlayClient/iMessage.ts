import { IChaosOverlayVoteOption } from './iVoteOption';

export interface IChaosOverlayClientMessage {
	request: 'CREATE' | 'END' | 'NO_VOTING_ROUND' | 'UPDATE';
	totalVotes: number;
	votingMode: 'MAJORITY' | 'PERCENTAGE';
	voteOptions: IChaosOverlayVoteOption[];
}
