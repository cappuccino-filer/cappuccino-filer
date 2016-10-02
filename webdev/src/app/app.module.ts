import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import 'hammerjs';

import { AppComponent } from './app.component';
import { VolumeListComponent } from './volume-list/volume-list.component';

import { MaterialModule } from '@angular/material';
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
    MaterialModule.forRoot()
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
