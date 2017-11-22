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
        this.service.fetchLabels().then(
            labels => {
                if(labels['Success']) {
                    this.service.fetchTests().then(
                        data => {
                            this.loading = false;
                            if(data['Success']) {
                                for(let i = 0; i < data['Data'].length; i++) {
                                    data['Data'][i]['Labels'] = [];
                                    for(let l = 0; l < labels['Data'].length; l++) {
                                        for(let li = 0; li < data['Data'][i]['LabelIds'].length; li++) {
                                            if(data['Data'][i]['LabelIds'][li] == labels['Data'][l]['Id']) {
                                                data['Data'][i]['Labels'].push(labels['Data'][l]['Name']);
                                            }
                                        }
                                    }
                                }
                            }
                            this.tests = data['Data'];
                        }
                    );
                }
            }
        )
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