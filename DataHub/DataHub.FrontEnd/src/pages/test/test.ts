import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute, Router } from '@angular/router';


@Component({
    templateUrl: './test.html'
})
export class Test {
     constructor(
        private service: Service,
        private route: ActivatedRoute,
        private router: Router
    ) { }

    public tests = [];
    public loading = true;

    ngOnInit() {
      this.service.fetchTests().then(
          data => {
            this.loading = false;
            console.log(data);
            if(data['Success'])
                this.tests = data['Data'];
          }
      );
    }

    public deleteTest(test) {
        this.service.deleteTest(test['Id']).then(
            data => {
                if(data['Success']) {
                    this.tests.splice(this.tests.indexOf(test), 1);
                }
            }
        );
    }
}