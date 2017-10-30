import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute, Router } from '@angular/router';


@Component({
    templateUrl: './newmodeltype.html'
})
export class NewModelType {
     constructor(
        private service: Service,
        private route: ActivatedRoute,
        private router: Router
    ) { }

    public modelType = {
        Name: "",
        Properties: [
        ]
    };

    public addProperty() {
        this.modelType.Properties.unshift({
            Name: null,
            Type: ""
        });
    }

    public deleteProperty(property) {
        this.modelType.Properties.splice(this.modelType.Properties.indexOf(property), 1);
    }

    public save() {
        this.service.addModelType(this.modelType).then(
            data => {
                console.log(data);
                if(data['Success'])
                    this.router.navigate(['/model']);
            }
        )
    }

    ngOnInit() {
      this.route.params.subscribe(
            params => {
                if(params['id'] != null) {
                    this.service.fetchModelType(params['id']).then(
                        modelTypeData => {
                            if(modelTypeData['Success']) {
                                console.log(modelTypeData);
                                this.modelType = modelTypeData['Data'];
                            }
                        }
                    )
                }
            });
    }
}