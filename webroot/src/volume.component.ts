import {Component, OnInit} from '@angular/core';
import {NavComponent} from './nav.component';
import {VolumeInformation} from './volume.information';
import {VolumeService} from './volume.service';

@Component({
    selector: 'conf-vol',
    template: `
    <h1>{{title}}</h1>
    <nav-conf></nav-conf>
    <article>
    <ul class="volumeclass" >
    <li *ngFor="let vol of volinfo">
	<span class="uuid"> {{vol.uuid}} </span>
    	<span class="mountpoint"> {{vol.mount}} </span>
	<input type="checkbox" [(ngModel)]="vol.tracking" />
    </li>
    </ul>
    <button type="button" (click)="doSubmit()">Submit</button>
    </article>
    `,
  styles:[`
    .selected {
      background-color: #CFD8DC !important;
      color: white;
    }
    .volumeclass {
      margin: 0 0 2em 0;
      list-style-type: none;
      padding: 0;
      width: 60em;
    }
    .volumeclass li {
      cursor: pointer;
      position: relative;
      left: 0;
      background-color: #607D8B;
      margin: .5em;
      padding: .3em 0;
      height: 1.6em;
      border-radius: 4px;
    }
    .volumeclass li.selected:hover {
      background-color: #BBD8DC !important;
      color: white;
    }
    .volumeclass li:hover {
      color: #607D8B;
      background-color: #DDD;
      left: .1em;
    }
    .volumeclass .text {
      position: relative;
      top: -3px;
    }
    .volumeclass .uuid {
      display: inline-block;
      font-family: monospace;
      color: white;
      padding: 0.8em 0.7em 0 0.7em;
      line-height: 1em;
      position: relative;
      left: -1px;
      top: -4px;
      height: 4em;
      margin-right: .8em;
      border-radius: 4px 0 0 4px;
    }
    .volumeclass .mountpoint{
      display: inline-block;
      color: white;
      font-family: monospace;
      height: 4.0em;
    }
    .volumeclass input[type='checkbox'] { float: right; }
  `],
  directives: [ NavComponent ],
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

	ngOnInit() {
		this.getVolumeInfo();
	}

	doSubmit() {
		this.volumeService.updateVolumeSettings(this.volinfo).then(volumes => this.volinfo = volumes);
	}
}
