import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute, Router } from '@angular/router';


@Component({
    templateUrl: './testresult.html'
})
export class TestResult {
     constructor(
        private service: Service,
        private route: ActivatedRoute,
        private router: Router
    ) { }

    public results = [];
    public loading = true;

    ngOnInit() {
        this.route.params.subscribe(
            params => {
                this.service.fetchResults(params['id']).then(
                    data => {
                        console.log(data);
                        if(data['Success']) {
                            for(let r = 0; r < data['Data'].length; r++) {
                                let correct = 0;
                                let incorrect = 0;
                                for(let d = 0; d < data['Data'][r]['DataSetResults'].length; d++) {
                                    let maxConfidenceIndex = 0;
                                    let maxConfidence = 0;
                                    for(let c = 0; c < data['Data'][r]['DataSetResults'][d]['Classifications'].length; c++) {
                                        let confidence = data['Data'][r]['DataSetResults'][d]['Classifications'][c]['Confidence']; 
                                        if(confidence > maxConfidence) {
                                            maxConfidenceIndex = c;
                                            maxConfidence = confidence;
                                        }
                                    }
                                    let classification = data['Data'][r]['DataSetResults'][d]['Classifications'][maxConfidenceIndex];
                                    console.log(classification);
                                    if(data['Data'][r]['DataSetResults'][d]['LabelIds'].indexOf(classification['LabelId']) != -1) {
                                        classification['IsCorrect'] = true;
                                        classification['IsIncorrect'] = false;
                                        data['Data'][r]['DataSetResults'][d]['IsCorrect'] = true;
                                        data['Data'][r]['DataSetResults'][d]['IsIncorrect'] = false;
                                        correct++;
                                    } else {
                                        classification['IsCorrect'] = false;
                                        classification['IsIncorrect'] = true;
                                        data['Data'][r]['DataSetResults'][d]['IsCorrect'] = false;
                                        data['Data'][r]['DataSetResults'][d]['IsIncorrect'] = true;
                                        incorrect++;
                                    }
                                }
                                data['Data'][r]['Correct'] = correct;
                                data['Data'][r]['Incorrect'] = incorrect;
                            }
                            this.loading = false;
                            this.results = data['Data'];
                        }
                    }
                );
            });
    }
}