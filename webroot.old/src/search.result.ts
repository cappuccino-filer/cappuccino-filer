export class SearchResult {
	name: string;
	path: string;

	constructor()
	{
	}
};

export class SearchReply {
	cat: string;
	result: string;
	reason: string;
	start: number;
	items: Array<SearchResult>;
	cache_cookie: string;

	constructor()
	{
	}
};
