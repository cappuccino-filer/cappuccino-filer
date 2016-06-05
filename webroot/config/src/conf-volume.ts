import {bootstrap}    from '@angular/platform-browser-dynamic';
import {VolumeComponent} from './volume.component';
import { HTTP_PROVIDERS } from '@angular/http';

bootstrap(VolumeComponent, [ HTTP_PROVIDERS ]);
