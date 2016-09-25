import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import 'hammerjs';

import { AppComponent } from './app.component';
import { VolumeListComponent } from './volume-list/volume-list.component';

import { MdInputModule } from '@angular2-material/input/input';
import { MdListModule } from '@angular2-material/list/list';
import { MdButtonModule } from '@angular2-material/button';
import { NameSearchComponent } from './name-search/name-search.component';

@NgModule({
  declarations: [
    AppComponent,
    VolumeListComponent,
    NameSearchComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule,
    MdInputModule.forRoot(),
    MdListModule.forRoot(),
    MdButtonModule.forRoot()
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
