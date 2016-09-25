/* tslint:disable:no-unused-variable */

import { TestBed, async, inject } from '@angular/core/testing';
import { NameSearchService } from './name-search.service';

describe('Service: NameSearch', () => {
  beforeEach(() => {
    TestBed.configureTestingModule({
      providers: [NameSearchService]
    });
  });

  it('should ...', inject([NameSearchService], (service: NameSearchService) => {
    expect(service).toBeTruthy();
  }));
});
