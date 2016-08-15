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

	constructor(private searchService : SearchService) {}

	public ngOnInit():void
	{
		this.rows = [];
		this.statusString = "";
	}

	public doSearch(regex : string):void
	{
		this.searchService.getMatchedFiles(regex).then(retobj => this.updateCache(retobj));
	}

	private updateCache(retobj: SearchReply):void
	{
		this.statusString = retobj.code;
		this.rows = retobj.items;
	};
};
