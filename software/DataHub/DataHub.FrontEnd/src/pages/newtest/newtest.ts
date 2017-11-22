import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute, Router } from '@angular/router';


@Component({
    templateUrl: './newtest.html'
})
export class NewTest {
     constructor(
        private service: Service,
        private route: ActivatedRoute,
        private router: Router
    ) { }

    public test = { LabelIds: [], TestDataSetIds: [], TrainingDataSetIds: [] };
    private alldatalist = [];
    public loadingLabels = true;
    public loadingData = true;
    public datalist = [];

    public labels = [12];
    public selectedLabels = [];

    ngOnInit() {
      this.service.fetchLabels().then(
          data => {
              this.loadingLabels = false;
              if(data['Success'])
                this.labels = data['Data'];
          }
      );

      this.service.fetchDataList().then(
            data => {
                this.loadingData = false;
                if(data['Success'])
                    this.alldatalist = data['Data'] as any[];
            }
        )
    }

    public save() {
        this.service.addTest(this.test).then(
            data => {
                if(data['Success'])
                    this.router.navigate(['/test']);
            }
        )
    }

    public selectLabel(label) {
        if(this.test.LabelIds.indexOf(label.Id) == -1)
        {
            this.test.LabelIds.push(label.Id);
        } else {
            
            this.test.LabelIds.splice(this.test.LabelIds.indexOf(label.Id), 1);
        }
        for(let i = 0; i < this.alldatalist.length; i++) {
            if(this.hasCommonIds(this.alldatalist[i].LabelIds as any[], this.test.LabelIds as any[])) {
                if(this.datalist.indexOf(this.alldatalist[i]) == -1) {
                    this.datalist.push(this.alldatalist[i]);
                    this.test.TestDataSetIds.push(this.alldatalist[i]['Id']);
                }
            } else {
                if(this.datalist.indexOf(this.alldatalist[i]) != -1) {
                    this.datalist.splice(this.datalist.indexOf(this.alldatalist[i]), 1);
                    this.test.TrainingDataSetIds.splice(this.test.TrainingDataSetIds.indexOf(this.alldatalist[i]['Id']), 1);
                    this.test.TestDataSetIds.splice(this.test.TestDataSetIds.indexOf(this.alldatalist[i]['Id']), 1);
                }
            }
        }
    }

    public hasCommonIds(a : any[], b : any[]) {
        for(let x = 0; x < a.length; x++)
            for(let y = 0; y < b.length; y++)
                if(a[x] == b[y])
                    return true;
        return false;
    }

    public selectTrainingData(dataset) {
        if(this.test.TrainingDataSetIds.indexOf(dataset.Id) == -1) {
            this.test.TrainingDataSetIds.push(dataset.Id);
            this.test.TestDataSetIds.splice(this.test.TestDataSetIds.indexOf(dataset.Id), 1);
        }
    }

    public selectTestData(dataset) {
        if(this.test.TestDataSetIds.indexOf(dataset.Id) == -1) {
            this.test.TestDataSetIds.push(dataset.Id);
            this.test.TrainingDataSetIds.splice(this.test.TrainingDataSetIds.indexOf(dataset.Id), 1);
        }
    }
}