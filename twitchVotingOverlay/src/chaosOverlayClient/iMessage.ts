import { IChaosOverlayVoteOption } from './iVoteOption';

export interface IChaosOverlayBaseClientMessage
{
	type : 'SET_VOTES' | 'SET_COLOR';
	messageData : string;
}

export interface IChaosOverlayColorClientMessage {
	colorR : number;
	colorG : number;
	colorB : number;
}

export interface IChaosOverlayVotingClientMessage {
	retainInitialVotes: boolean;
	request: 'CREATE' | 'END' | 'NO_VOTING_ROUND' | 'UPDATE';
	totalVotes: number;
	voteOptions: IChaosOverlayVoteOption[];
	votingMode: 'MAJORITY' | 'PERCENTAGE';
}
