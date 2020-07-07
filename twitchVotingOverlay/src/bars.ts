export class Bar {
	public static readonly BAR_WIDTH = 350;
	public static readonly BAR_HEIGHT = 30;

	private bar: HTMLDivElement;
	private barWrapper: HTMLDivElement;
	private labelMatch: HTMLSpanElement;
	private labelText: HTMLSpanElement;
	private labelVotes: HTMLSpanElement;
	private labelWrapper: HTMLDivElement;

	public constructor(label: string, votes: number) {
		// Create DOM elements
		this.bar = document.createElement('div');
		this.barWrapper = document.createElement('div');
		this.labelMatch = document.createElement('span');
		this.labelWrapper = document.createElement('div');
		this.labelText = document.createElement('span');
		this.labelVotes = document.createElement('span');
		// Assign classes
		this.bar.classList.add('bar');
		this.barWrapper.classList.add('barWrapper');
		this.labelWrapper.classList.add('labelWrapper');

		this.matches = [];
		// Assemble the bar element
		this.labelWrapper.append(this.labelMatch, this.labelText, this.labelVotes);
		this.barWrapper.append(this.labelWrapper);
		this.barWrapper.append(this.bar);

		this.bar.style.height = `${Bar.BAR_HEIGHT}px`;
		this.barWrapper.style.height = `${Bar.BAR_HEIGHT}px`;
		this.barWrapper.style.width = `${Bar.BAR_WIDTH}px`;

		// Set the label content
		this.label = label;
		this.votes = votes;
	}

	public getBarWrapper(): HTMLDivElement {
		return this.barWrapper;
	}

	public set label(label: string) {
		this.labelText.innerText = label;
	}

	public set votes(votes: number) {
		this.labelVotes.innerText = votes.toString();
	}

	public set barWidth(width: string) {
		this.bar.style.width = width;
	}

	public set matches(matches: string[]) {
		this.labelMatch.innerText = matches.join('/') + '.';
	}
}
