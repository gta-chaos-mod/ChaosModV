import { ChaosOverlayClient } from './chaosOverlayClient/client';
import { Bar } from './bars';
import { IChaosOverlayClientMessage } from './chaosOverlayClient/iMessage';

let bars: Bar[] = [];
let totalVotes = 0;

// Get DOM elements
const BAR_CONTAINER = document.getElementById('barContainer');
const TOTAL_VOTES = document.getElementById('totalVotes');

const WS_CLIENT = new ChaosOverlayClient('ws://localhost:9091');

WS_CLIENT.addCreateVoteListener(createVote);
WS_CLIENT.addUpdateVoteListener(updateVote);
WS_CLIENT.addEndVoteListener(() => console.log('vote should have ended'));

function createVote(message: IChaosOverlayClientMessage): void {
	// Remove old bars from DOM
	bars.forEach(bar => bar.getBarWrapper().remove());
	// Update bars
	bars = message.voteOptions.map(
		voteOption => new Bar(voteOption.label, voteOption.value.toString())
	);
	updateBarContainer();
	// Update the total votes counter
	totalVotes = message.totalVotes;
	updateTotalVotes();
}

function updateVote(message: IChaosOverlayClientMessage): void {
	const { voteOptions, totalVotes: newTotalVotes } = message;
	// Check if the length of the vote options is the same as the bar amount.
	// If not, an error ocurred and we just treat the update as create
	if (bars.length !== voteOptions.length) {
		console.error(`bar amount is not equal to vote options length, treating as new vote`);
		createVote(message);
	} else {
		voteOptions.forEach((voteOption, index) => {
			const bar = bars[index];
			const BAR_WIDTH = newTotalVotes > 0 ? (Bar.BAR_WIDTH / newTotalVotes) * voteOption.value : 0;

			if (message.votingMode === 'PERCENTAGE') {
				let barValue: number;
				// We can't divide by total votes if it is null
				if (newTotalVotes === 0) barValue = 0;
				else barValue = Math.floor((voteOption.value / newTotalVotes) * 100);
				bar.votes = `${barValue}%`;
			} else bar.votes = voteOption.value.toString();

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
