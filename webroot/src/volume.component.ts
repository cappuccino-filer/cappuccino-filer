import { Component, OnInit } from '@angular/core';
import { CORE_DIRECTIVES, FORM_DIRECTIVES, NgClass, NgIf } from '@angular/common';
import { NavComponent } from './nav.component';
import { VolumeInformation } from './volume.information';
import { VolumeService } from './volume.service';

@Component({
	selector: 'conf-vol',
	templateUrl : '/html/volume.component.html',
	styleUrls: ['assets/volume.component.css'],
	//template: `<h1>{{title}}</h1>`,
	//styles: [`.selected { background-color: #CFD8DC !important; color: white; }`],
	directives: [ NavComponent, NgClass, NgIf, CORE_DIRECTIVES, FORM_DIRECTIVES ],
	providers: [ VolumeService ]
})

export class VolumeComponent implements OnInit {
	title = 'Select Tracking Volumes';
	volinfo : VolumeInformation[];

	constructor(private volumeService: VolumeService) { }

	getVolumeInfo()
	{
		this.volumeService.getVolumeInfo().then(volumes => this.volinfo = volumes);
	}

	public ngOnInit():void
	{
		this.getVolumeInfo();
	}

	public doSubmit():void
	{
		this.volumeService.updateVolumeSettings(this.volinfo)
				  .then(volumes => this.volinfo = volumes);
	}
}
