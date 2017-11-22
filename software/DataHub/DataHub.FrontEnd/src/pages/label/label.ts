import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute } from '@angular/router';


@Component({
    templateUrl: './label.html'
})
export class Label {
    public datalist = [];
    public selected = null;
    public loading = true;
    public loadingLabels = true;
    public labels = [];
    public newLabelName = "";

    public dataLabels = [{ DataSetId: 1, LabelId: 1 }]
    
    public selectedLabels = [];

    public constructor(
        private service : Service
    ) {}

    ngOnInit() {
        this.service.fetchLabels().then(
            data => {
                this.labels = data['Data'] as any[];
                this.loadingLabels = false;
                this.service.fetchDataList().then(
                    datal => {
                        this.datalist = datal['Data'] as any[];
                        this.loading = false;
                    }
                );   
            }
        )
    }

    public addNewLabel() {
        this.service.addLabel({ Name: this.newLabelName }).then(
            data => {
                if(data['Success']) {
                    this.labels.unshift(data['Data']);
                    this.newLabelName = "";
                }
            }
        )
    }

    public deleteLabel(label) {
        this.service.deleteLabel(label['Id']).then(
            data => {
                if(data['Data']) {
                    this.labels.splice(this.labels.indexOf(label), 1);
                }
            }
        );
    }

    public checkEnter(event) {
        if((event as KeyboardEvent).key == "Enter")
            this.addNewLabel();
    }

    public selectDataset(dataset) {
        this.selected = dataset; 
        this.loadingLabels = true;
        this.service.fetchDataSetLabels(this.selected['Id']).then(
            data => {
                this.loadingLabels = false;
                this.selectedLabels = [];
                for(let i = 0; i < this.labels.length; i++) {
                    for(let y = 0; y < data['Data'].length; y++) {
                        if(data['Data'][y]['Id'] == this.labels[i]['Id']) {
                            this.selectedLabels.push(this.labels[i]);
                        }
                    }
                }
            }
        )
    }

    public selectLabel(label) {
        if(this.selectedLabels.indexOf(label) == -1)
        {
            this.service.addDataSetLabel(this.selected['Id'], label).then(
                data => {
                    if(data['Success'])
                        this.selectedLabels.push(label);
                }
            );
        } else {
            this.service.deleteDataSetLabel(this.selected['Id'], label['Id']).then(
                data => {
                    console.log(data);
                    if(data['Success'])
                        this.selectedLabels.splice(this.selectedLabels.indexOf(label), 1);
                }
            );
        }
    }
}