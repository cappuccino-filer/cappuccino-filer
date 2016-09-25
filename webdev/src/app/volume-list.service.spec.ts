/* tslint:disable:no-unused-variable */

import { TestBed, async, inject } from '@angular/core/testing';
import { VolumeListService } from './volume-list.service';

describe('Service: VolumeList', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [VolumeListService]
    });
  });

  it('should ...', inject([VolumeListService], (service: VolumeListService) => {
    expect(service).toBeTruthy();
  }));
});
