import { ChaosOverlayClient } from './chaosOverlayClient/client';
import { Bar } from './bars';
import { IChaosOverlayClientMessage } from './chaosOverlayClient/iMessage';
import { BarOverlay } from './barOverlay';

// Get DOM elements
const BAR_CONTAINER = document.getElementById('barContainer') as HTMLDivElement | null;
const TOTAL_VOTES = document.getElementById('totalVotes') as HTMLDivElement | null;

if (BAR_CONTAINER === null) throw new Error('could not find bar container in DOM');
if (TOTAL_VOTES === null) throw new Error('could not find total votes element in DOM');

const OVERLAY_CLIENT = new ChaosOverlayClient('ws://localhost:9091');

OVERLAY_CLIENT.addUpdateVoteListener(message => {
	TOTAL_VOTES.innerText = `Total Votes: ${message.totalVotes}`;
});

new BarOverlay(BAR_CONTAINER, OVERLAY_CLIENT);
