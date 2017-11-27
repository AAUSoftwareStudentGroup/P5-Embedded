import { Component } from '@angular/core';
import { ActivatedRoute, Router, NavigationStart, NavigationEnd, NavigationError, NavigationCancel, RoutesRecognized } from '@angular/router';

@Component({
  selector: 'app-root',
  templateUrl: './app.template.html'
})
export class AppComponent { 
  constructor(
        private route: ActivatedRoute,
        private router: Router
    ) { }
private currentPath = "/data";
    ngOnInit() {
        this.router.events.subscribe(event => {
          if(event instanceof NavigationEnd)
            if(event.url == '/' || event.url.indexOf('dataset') != -1)
              this.currentPath = "/data";
            else if(event.url.indexOf('newmodel') != -1 || event.url.indexOf('newmodeltype') != -1)
              this.currentPath = '/model';
            else if(event.url == "/newtest" || event.url == "/testresult" || event.url.indexOf('test') != -1)
              this.currentPath = "/test";
            else
              this.currentPath = event.url;
        });
    }

  public pages = [
    { name: "New", path: "/new", icon: "/assets/gfx/new-icon.png" },
    { name: "Data", path: "/data", icon: "/assets/gfx/data-icon.png" },
    { name: "Label", path: "/label", icon: "/assets/gfx/label-icon.png" },
    { name: "Model", path: "/model", icon: "/assets/gfx/models-icon.png" },
    { name: "Test", path: "/test", icon: "/assets/gfx/test-icon.png" },
    { name: "Monitor", path: "/monitor", icon: "/assets/gfx/monitor-icon.png" },
  ];
}