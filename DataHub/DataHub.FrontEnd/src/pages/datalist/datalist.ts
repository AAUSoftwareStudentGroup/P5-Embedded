import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute } from '@angular/router';


@Component({
    templateUrl: './datalist.html'
})
export class DataList {
    public datalist = [];
    public loading = true;

    constructor(
        private service: Service,
        private route: ActivatedRoute
    ) { }

    ngOnInit() {
        this.service.fetchDataList().then(
            data => {
                this.datalist = data['Data'] as any[];
                this.loading = false;
            }
        )
    }

    private deleteDataSet(dataset)
    {
        this.service.deleteDataSet(dataset.Id).then(
            data => {
                this.datalist.splice(this.datalist.indexOf(dataset), 1);
            }
        );
    }

    public downloadDataSet(dataset)
    {
        window.location.href = 'http://p5datahub.azurewebsites.net/api/dataset/' + dataset.Id + '/file';
    }
}