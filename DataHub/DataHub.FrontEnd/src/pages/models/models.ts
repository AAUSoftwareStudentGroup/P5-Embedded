import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute } from '@angular/router';


@Component({
    templateUrl: './models.html'
})
export class Models {
    public modelTypes = [];
    public models = [];
    public loadingModels = true;
    public loadingModelTypes = true;
    constructor(
        private service: Service,
        private route: ActivatedRoute
    ) { }

    ngOnInit() {
      this.service.fetchModelTypes().then(
          data => {
              this.loadingModelTypes = false;
              if(data['Success'])
                this.modelTypes = data['Data'];
          }
      )
      this.service.fetchModels().then(
          data => {
              this.loadingModels = false;
              if(data['Success'])
                this.models = data['Data'];
          }
      )
    }

    public deleteModel(model) {
        this.service.deleteModel(model['Id']).then(
            data => {
                if(data['Success']) {
                    this.models.splice(this.models.indexOf(model), 1);
                }
            }
        )
    }

    public deleteModelType(modelType) {
        this.service.deleteModelType(modelType['Id']).then(
            data => {
                if(data['Success']) {
                    this.modelTypes.splice(this.modelTypes.indexOf(modelType), 1);
                }
            }
        )
    }
}