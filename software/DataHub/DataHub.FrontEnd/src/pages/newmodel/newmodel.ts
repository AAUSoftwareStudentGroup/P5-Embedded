import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute, Router } from '@angular/router';


@Component({
    templateUrl: './newmodel.html'
})
export class NewModel {
     constructor(
        private service: Service,
        private route: ActivatedRoute,
        private router: Router
    ) { }

    public model = {
        Name: "",
        TypeId: "",
        Parameters: []
    };

    public modelTypes = [];

    ngOnInit() {
        this.route.params.subscribe(
            params => {
            this.service.fetchModelTypes().then(
                data => {
                    if(data['Success']) {
                        this.modelTypes = data['Data'];
                        if(params['id'] != null) {
                            this.service.fetchModel(params['id']).then(
                                modelData => {
                                    if(modelData['Success']) {
                                        console.log(modelData);
                                        this.model = modelData['Data'];
                                    }
                                }
                            )
                        }
                    }
                }
            )
        });
    }

    public typeChanged(id) {
        this.model.TypeId = id;
        this.model.Parameters = [];
        this.service.fetchModelTypeProperties(id).then(
            data => {
                if(data['Success']) {
                    for(let i = 0; i < data['Data'].length; i++) {
                        this.model.Parameters.push({
                            PropertyId: data['Data'][i]['Id'],
                            Name: data['Data'][i]['Name'],
                            Type: data['Data'][i]['Type'],
                            Value: ""
                        });
                    }
                }
            }
        )
    }

    public save() {
        this.service.addModel(this.model).then(
            data => {
                if(data['Success']) {
                    this.router.navigate(['/model']);
                }
            }
        )
    }

   


    
}