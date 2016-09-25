import { Component, OnInit } from '@angular/core';
import { NameSearchService } from '../name-search.service';
import { SearchResult, SearchReply } from '../search-result';

@Component({
  selector: 'app-name-search',
  templateUrl: './name-search.component.html',
  styleUrls: ['./name-search.component.css'],
  providers: [ NameSearchService ]
})
export class NameSearchComponent implements OnInit {
	title = "Search by Name Component";

	public rows : SearchResult[];
	public statusString : string = "";
	public nextPageText : string = "";
	public disableNextPage : boolean = true;
	
	private nresults : number = 100;
	private current_regex : string = "";
	private cache_cookie : string = "";
	
	constructor(private searchService : NameSearchService) {}

	public ngOnInit():void
	{
		this.rows = [];
		this.statusString = "";
	}

	public doSearch(regex : string):void
	{
		this.current_regex = regex;
		this.searchService.getMatchedFiles(regex, 0, this.nresults, "")
				  .then(retobj => this.refreshCache(retobj));
	}
	
	public doNextSearch():void
	{
		this.searchService.getMatchedFiles(this.current_regex,
						   this.rows.length,
						   this.nresults,
						   this.cache_cookie)
				  .then(retobj => this.appendCache(retobj));
	}

	private refreshCache(retobj: SearchReply):void
	{
		this.rows = retobj.items;
		this.updateStatus(retobj);
	};
	
	private appendCache(retobj: SearchReply):void
	{
		this.rows = this.rows.concat(retobj.items);
		this.updateStatus(retobj);
	}
	
	private updateStatus(retobj: SearchReply):void
	{
		this.statusString = retobj.result;
		this.cache_cookie = retobj.cache_cookie;
		if (this.statusString == 'OK Partial') {
			this.disableNextPage = false;
			this.nextPageText = 'More';
		} else if (this.statusString == 'OK') {
			this.disableNextPage = true;
			this.nextPageText = 'Done';
		} else {
			this.disableNextPage = true;
			this.nextPageText = 'Error: ' + this.statusString;
		}
	}
}
