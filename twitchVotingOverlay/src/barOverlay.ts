import { IChaosOverlayClient } from './chaosOverlayClient/iClient';
import { IChaosOverlayClientMessage } from './chaosOverlayClient/iMessage';

const ANIMATION_DELAY_DELTA = 100;
const ANIMATION_LENGTH = 600;

class Bar {
	private _isAnimating = false;
	private bar: HTMLDivElement;
	private barProgression: HTMLDivElement;
	private container: HTMLDivElement;
	private labelLabel: HTMLSpanElement;
	private labelMatch: HTMLSpanElement;
	private labelValue: HTMLSpanElement;
	private labelContainer: HTMLDivElement;

	public constructor(container: HTMLDivElement) {
		this.container = container;

		// Construct bar
		this.bar = document.createElement('div');
		this.barProgression = document.createElement('div');
		this.labelContainer = document.createElement('div');
		this.labelLabel = document.createElement('span');
		this.labelMatch = document.createElement('span');
		this.labelValue = document.createElement('span');

		this.bar.classList.add('bar');
		this.barProgression.classList.add('progression');
		this.labelContainer.classList.add('labelContainer');

		this.labelContainer.append(this.labelMatch, this.labelLabel, this.labelValue);

		this.bar.append(this.barProgression);
		this.bar.append(this.labelContainer);
		this.container.append(this.bar);
	}

	public set label(value: string) {
		this.labelLabel.innerText = value;
	}
	public set match(value: string) {
		this.labelMatch.innerText = value;
	}
	public set value(value: string) {
		this.labelValue.innerText = value;
	}
	public set width(value: string) {
		this.barProgression.style.width = value;
	}

	public get isAnimating(): boolean {
		return this._isAnimating;
	}

	public fadeIn(duration: number, delay: number = 0): void {
		this.bar.style.animationDelay = `${delay}ms`;
		this.bar.style.animationDuration = `${duration}ms`;
		this.bar.classList.add('slideIn');
		this.bar.classList.remove('slideOut');
	}

	public fadeOut(duration: number, delay: number = 0): void {
		this.bar.style.animationDelay = `${delay}ms`;
		this.bar.style.animationDuration = `${duration}ms`;
		this.bar.classList.add('slideOut');
		this.bar.classList.remove('slideIn');
	}
}

export class BarOverlay {
	private container: HTMLDivElement;
	private bars: Bar[] = [];

	public constructor(container: HTMLDivElement, overlayClient: IChaosOverlayClient) {
		this.container = container;

		// Bind listener methods
		this.onCreateVote = this.onCreateVote.bind(this);
		this.onEndVote = this.onEndVote.bind(this);
		this.onUpdateVote = this.onUpdateVote.bind(this);

		// Add listener to the overlay client
		overlayClient.addCreateVoteListener(this.onCreateVote);
		overlayClient.addEndVoteListener(this.onEndVote);
		overlayClient.addUpdateVoteListener(this.onUpdateVote);
	}

	private onCreateVote(): void {
		this.bars.forEach((bar, index) => {
			const ANIMATION_DELAY = index * ANIMATION_DELAY_DELTA;
			bar.fadeOut(ANIMATION_LENGTH, ANIMATION_DELAY);
			setTimeout(() => bar.fadeIn(ANIMATION_LENGTH), ANIMATION_LENGTH + ANIMATION_DELAY);
		});
	}
	private onEndVote(): void {}
	private onUpdateVote(message: IChaosOverlayClientMessage): void {
		const { totalVotes, voteOptions, votingMode } = message;

		if (voteOptions.length !== this.bars.length) {
			while (this.container.firstChild) this.container.removeChild(this.container.firstChild);
			this.bars = voteOptions.map((_, index) => new Bar(this.container));
		}

		for (let i = 0; i < voteOptions.length; i++) {
			const BAR = this.bars[i];
			const VOTE_OPTION = voteOptions[i];

			if (BAR.isAnimating) continue;

			let percentage = 0;
			if (VOTE_OPTION.value === 0) percentage = 0;
			else percentage = Math.floor((VOTE_OPTION.value / totalVotes) * 100);

			BAR.label = VOTE_OPTION.label;
			BAR.match = VOTE_OPTION.matches.join('/').concat('.');

			if (votingMode === 'MAJORITY') {
				BAR.value = VOTE_OPTION.value.toString();
			} else if (votingMode === 'PERCENTAGE') BAR.value = `${percentage}%`;

			BAR.width = `${percentage}%`;
		}
	}
}
