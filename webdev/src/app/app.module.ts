import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';
import 'hammerjs';

import { AppComponent } from './app.component';
import { VolumeListComponent } from './volume-list/volume-list.component';

//import { MdInputModule } from '@angular/material/input/input';
//import { MdListModule } from '@angular/material/list/list';
//import { MdButtonModule } from '@angular/material/button';
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
    //MdInputModule.forRoot(),
    //MdListModule.forRoot(),
    //MdButtonModule.forRoot()
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
