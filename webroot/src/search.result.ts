export class SearchResult {
	name: string;
	path: string;

	constructor()
	{
	}
};

export class SearchReply {
	cat: string;
	code: string;
	reason: string;
	items: Array<SearchResult>;

	constructor()
	{
	}
};
