import { Component, OnInit } from '@angular/core';
import { VolumeListService } from '../volume-list.service';
import { VolumeInformation } from '../volume-information';

@Component({
  selector: 'app-volume-list',
  templateUrl: './volume-list.component.html',
  styleUrls: ['./volume-list.component.css'],
  providers: [ VolumeListService ]
})

export class VolumeListComponent implements OnInit {
	title = 'Select Tracking Volumes';
	volinfo : VolumeInformation[];

	constructor(private volumeService : VolumeListService) { }

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
