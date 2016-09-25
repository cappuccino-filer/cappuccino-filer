import { WebrootPage } from './app.po';

describe('webroot App', function() {
  let page: WebrootPage;

  beforeEach(() => {
    page = new WebrootPage();
  });

  it('should display message saying app works', () => {
    page.navigateTo();
    expect(page.getParagraphText()).toEqual('app works!');
  });
});
