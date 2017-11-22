import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute } from '@angular/router';


@Component({
    templateUrl: './dataset.html'
})
export class DataSet {
    public chart;
    public name = 'Loading';

    public dataChartOptions = {
        chart: {
            zoomType: 'xy',
            panning: true,
            panKey: 'shift',
            width: null
        },

        boost: {
            useGPUTranslations: true
        },

        title: {
            text: 'Loading'
        },

        subtitle: {
            text: 'Loading data'
        },

        plotOptions: {
            series: {
                animation: false,
                states: {
                    hover: {
                        enabled: false
                    }
                },
                marker: {
                    enabled: false
                }
            }
        },

        tooltip: { enabled: false },

        series: [
            { name: "X", data: [] },
            { name: "Y", data: [] },
            { name: "Z", data: [] },
            { name: "RX", data: [] },
            { name: "RY", data: [] },
            { name: "RZ", data: [] },
        ]
    };

    constructor(
        private service: Service,
        private route: ActivatedRoute
    ) { }

    ngOnInit() {
        this.paramsSubscription = this.route.params.subscribe(
            params => {
                this.service.fetchDataSet(params['id']).then(
                    data => {
                        this.name = data['Data']['Name'] as string;
                        this.chart.setTitle({text: this.name});
                    }
                );
            });
    }

    public saveInstance(chartInstance) {
        this.chart = chartInstance;
        this.refreshChartData();
    }

    private paramsSubscription;
    public loading = true;

    private refreshChartData(){
        this.paramsSubscription = this.route.params.subscribe(
            params => {
                this.service.fetchData(params['id']).then(
                    data => {
                        for(let s = 0; s < this.chart.series.length; s++)
                        {
                            let serie = [];
                            for(let i = 0; i < data['Data'].length; i++)
                            {
                                let na = this.chart.series[s].name;
                                let val = data['Data'][i][na];
                                serie.push([data['Data'][i]['Time'], val]);
                            }

                            this.chart.series[s].setData(serie);
                        }
                        this.loading = false;
                        this.chart.setTitle(null,  { text: data['Data'].length + " data points loaded." });
                    }  
                );
            }
        );
    }
}