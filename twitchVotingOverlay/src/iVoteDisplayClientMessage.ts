export interface IVoteDisplayClientMessage {
	request: 'CREATE' | 'UPDATE';
	totalVotes: number;
	voteOptions: {
		label: string;
		votes: number;
	}[];
}
