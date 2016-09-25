import { Component, OnInit } from '@angular/core';
import { CORE_DIRECTIVES, FORM_DIRECTIVES, NgClass, NgIf } from '@angular/common';
import { SearchResult, SearchReply } from './search.result';
import { SearchService } from './search.service';
import { MD_RIPPLE_DIRECTIVES } from '@angular2-material/core';
import { MdButton } from '@angular2-material/button';
import { MdInput } from '@angular2-material/input';
import { MD_LIST_DIRECTIVES } from '@angular2-material/list';

@Component({
	selector : 'search-complex',
	templateUrl : '/html/search.html',
	styleUrls: [`assets/search.component.css`],
	//template: `<h1>{{title}}</h1>`,
	//styles: [`.selected { background-color: #CFD8DC !important; color: white; }`],
	directives: [
		SearchComponent,
		NgClass, NgIf,
		CORE_DIRECTIVES, FORM_DIRECTIVES,
		MdButton, MdInput,
		MD_LIST_DIRECTIVES,
		MD_RIPPLE_DIRECTIVES,
	],
	providers: [SearchService]
})

export class SearchComponent implements OnInit {
	title = "Search Component";
	// Core API required by stripped html template from ng-table
	public rows : SearchResult[];
	public statusString : string = "";
	public nextPageText : string = "";
	public disableNextPage : boolean = true;
	
	private nresults : number = 100;
	private current_regex : string = "";
	private cache_cookie : string = "";

	constructor(private searchService : SearchService) {}

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
};
