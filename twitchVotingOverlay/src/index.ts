import { ChaosWSClient } from './chaosWSClient/chaosWSClient';
import { Bar } from './bars';
import { IMessage } from './chaosWSClient/iMessage';

let bars: Bar[] = [];
let totalVotes = 0;

// Get DOM elements
const BAR_CONTAINER = document.getElementById('barContainer');
const TOTAL_VOTES = document.getElementById('totalVotes');

const WS_CLIENT = new ChaosWSClient('ws://localhost:9091');

WS_CLIENT.addCreateVoteListener(createVote);
WS_CLIENT.addUpdateVoteListener(updateVote);

function createVote(message: IMessage): void {
	// Remove old bars from DOM
	bars.forEach(bar => bar.getBarWrapper().remove());
	// Update bars
	bars = message.voteOptions.map(voteOption => new Bar(voteOption.label, voteOption.votes));
	updateBarContainer();
	// Update the total votes counter
	totalVotes = message.totalVotes;
	updateTotalVotes();
}

function updateVote(message: IMessage): void {
	const { voteOptions, totalVotes: newTotalVotes } = message;
	// Check if the length of the vote options is the same as the bar amount.
	// If not, an error ocurred and we just treat the update as create
	console.log(message);
	if (bars.length !== voteOptions.length) {
		console.error(`bar amount is not equal to vote options length, treating as new vote`);
		createVote(message);
	} else {
		voteOptions.forEach((voteOption, index) => {
			const bar = bars[index];
			const BAR_WIDTH =
				newTotalVotes > 0 ? (Bar.BAR_WIDTH / message.totalVotes) * voteOption.votes : 0;
			bar.votes = voteOption.votes;
			bar.barWidth = `${BAR_WIDTH}px`;
			bar.matches = voteOption.matches;
		});
		// Update total votes
		totalVotes = newTotalVotes;
		updateTotalVotes();
	}
}

function updateBarContainer(): void {
	if (BAR_CONTAINER === null) throw new Error('could not find bar container in DOM');
	BAR_CONTAINER.append(...bars.map(bar => bar.getBarWrapper()));
}

function updateTotalVotes(): void {
	if (TOTAL_VOTES === null) throw new Error('could not find total votes in DOM');
	TOTAL_VOTES.innerText = `Total Votes: ${totalVotes.toString()}`;
}
