/**
 * System configuration for Angular 2 apps
 * Adjust as necessary for your application needs.
 */
(function(global) {

  // map tells the System loader where to look for things
  var map = {
    'nav':                        'app', // 'dist',
    'conf-volume':                'app',
    'search-complex':		  'app',
    '@angular':                   'node_modules/@angular',
    '@angular2-material':         'node_modules/@angular2-material',
    'rxjs':                       'node_modules/rxjs',
  };

  // packages tells the System loader how to load when no filename and/or no extension
  var packages = {
    'nav':                        { main: 'config-main.js',  defaultExtension: 'js' },
    'conf-volume':                { main: 'conf-volume.js',  defaultExtension: 'js' },
    'search-complex':             { main: 'search-app.js',  defaultExtension: 'js' },
    'rxjs':                       { defaultExtension: 'js' },
  };

  var ngPackageNames = [
    'common',
    'compiler',
    'core',
    'http',
    'forms',
    'platform-browser',
    'platform-browser-dynamic',
    'router',
    'router-deprecated',
    'upgrade',
  ];

  // Individual files (~300 requests):
  function packIndex(pkgName) {
    packages['@angular/'+pkgName] = { main: 'index.js', defaultExtension: 'js' };
  }

  // Bundled (~40 requests):
  function packUmd(pkgName) {
    packages['@angular/'+pkgName] = { main: '/bundles/' + pkgName + '.umd.js', defaultExtension: 'js' };
  };

  var setPackageConfig = System.packageWithIndex ? packIndex : packUmd;

  // Add package entries for angular packages
  ngPackageNames.forEach(setPackageConfig);

  const materialPkgs = [
    'core',
    'button',
    'input',
    'list',
  ];

  function materialMainFile(pkgName) {
    packages['@angular2-material/' + pkgName] = {main: pkgName+'.js', defaultExtension: 'js' };
  }

  materialPkgs.forEach(materialMainFile);

  var config = {
    map: map,
    packages: packages
  }

  System.config(config);

})(this);
