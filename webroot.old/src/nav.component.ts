import {Component} from '@angular/core';
import {VolumeComponent} from './volume.component';

export class ConfigItem {
	name: string;
	url: string;
	cssclass: string;
}

@Component({
    selector: 'nav-conf',
    template: `
    <nav>Configuration Navigator
    <ul class="configitems" >
    <li *ngFor="let config of confitems"
        [class.selected]="config === selectedConfigItem"
	(click)="onSelect(config)">
	<span class="badge"> {{config.name}} </span>
    </li>
    </ul>
    </nav>
    `,
  styles:[`
    .selected {
      background-color: #CFD8DC !important;
      color: white;
    }
    .configitems {
      margin: 0 0 2em 0;
      list-style-type: none;
      padding: 0;
      width: 15em;
    }
    .configitems li {
      cursor: pointer;
      position: relative;
      left: 0;
      background-color: #607D8B;
      margin: .5em;
      padding: .3em 0;
      height: 1.6em;
      border-radius: 4px;
    }
    .configitems li.selected:hover {
      background-color: #BBD8DC !important;
      color: white;
    }
    .configitems li:hover {
      color: #607D8B;
      background-color: #DDD;
      left: .1em;
    }
    .configitems .text {
      position: relative;
      top: -3px;
    }
    .configitems .badge {
      display: inline-block;
      font-size: small;
      color: white;
      padding: 0.8em 0.7em 0 0.7em;
      line-height: 1em;
      position: relative;
      left: -1px;
      top: -4px;
      height: 1.8em;
      margin-right: .8em;
      border-radius: 4px 0 0 4px;
    }
  `]
})

/*
@Component({
    selector: 'my-nav',
    template: '<nav> <div> <a href="/config/index.html" title="Test"></a> </div> </nav>'
})
*/

export class NavComponent {
	title = 'Tour of Configurations';
	confitems = CONFITEMS;
	selectedConfigItem : ConfigItem;

	onSelect(config: ConfigItem)
	{
		this.selectedConfigItem = config;
		if (config.url) {
			location.href = config.url;
		}
		/*
		if (config.cssclass) {
			document.getElementById("master").className = config.cssclass;
		}
	       */
	}
}

var CONFITEMS : ConfigItem[] = [
	{ name: 'Volumes', url: 'volconf.html', cssclass: 'conf-vol' },
	{ name: 'Return to file-manager', url: '/browse/index.html', cssclass: 'conf-return'},
];
