import { Injectable } from '@angular/core';
import { Headers, Http } from '@angular/http';
import { SearchResult, SearchReply } from './search.result';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class SearchService {
	private serviceUrl = '/api/locate';
	private headers = new Headers({
			'Content-Type': 'application/json'});

	constructor(private http: Http) { }

	getMatchedFiles(regex:string) : Promise<SearchReply> {
		let json = JSON.stringify(
			{
				cat : "byname",
				matcher : "regex",
				pattern : regex
			});

		return this.http.post(this.serviceUrl, json, {headers: this.headers})
			.toPromise()
			.then(response => response.json())
			.catch(this.handleError);
	}

	private handleError(error: any) {
		console.error('An error occurred', error);
		return Promise.reject(error.message || error);
	}
}
