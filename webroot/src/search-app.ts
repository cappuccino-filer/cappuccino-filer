import {NgModule, ApplicationRef} from '@angular/core';
import {bootstrap}    from '@angular/platform-browser-dynamic';
import {SearchComponent} from './search.component';
import { HTTP_PROVIDERS } from '@angular/http';

bootstrap(SearchComponent, [ HTTP_PROVIDERS ]);
