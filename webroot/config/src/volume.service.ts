import { Injectable } from '@angular/core';
import { VolumeInformation } from './volume.information';
import { VOLUMES } from './mock-volumes';
import { Headers, Http } from '@angular/http';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class VolumeService {
	private serviceUrl = '/api/volume';

	constructor(private http: Http) { }

	getVolumeInfo() : Promise<VolumeInformation[]> {
		return this.http.get(this.serviceUrl)
			.toPromise()
			.then(response => response.json().volumelist)
			.catch(this.handleError);
	}

	updateVolumeSettings(volumelist : VolumeInformation[]) : Promise<VolumeInformation[]> {
		let headers = new Headers({
			'Content-Type': 'application/json'});

		return this.http
			.post(this.serviceUrl, JSON.stringify({volumelist: volumelist}), {headers : headers})
			.toPromise()
			.then(response => response.json().volumelist)
			.catch(this.handleError);
	}

	private handleError(error: any) {
		console.error('An error occurred', error);
		return Promise.reject(error.message || error);
	}
}
