import {Component, OnInit} from '@angular/core';
import {CORE_DIRECTIVES, FORM_DIRECTIVES, NgClass, NgIf} from '@angular/common';
import { SearchResult, SearchReply } from './search.result';
import { SearchService } from './search.service';

@Component({
	selector : 'search-complex',
	templateUrl : '/html/search.html',
	styleUrls: [`assets/search.component.css`],
	//template: `<h1>{{title}}</h1>`,
	//styles: [`.selected { background-color: #CFD8DC !important; color: white; }`],
	directives: [SearchComponent, NgClass, NgIf, CORE_DIRECTIVES, FORM_DIRECTIVES],
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
