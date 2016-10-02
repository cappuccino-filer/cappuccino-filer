import { Component, ViewEncapsulation } from '@angular/core';
import { MdIcon, MdIconRegistry } from '@angular/material';

@Component({
  selector: 'app-root',
  templateUrl: './app.component.html',
  styleUrls: ['./app.component.css'],
  viewProviders: [ MdIconRegistry ],
  encapsulation: ViewEncapsulation.None,
})
export class AppComponent {
  public title = 'AppComponent loaded';
}
