import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute } from '@angular/router';
import { Label } from '../label/label';


@Component({
    templateUrl: './monitor.html'
})

export class Monitor {
    public chart;
    public name = 'Loading';
    public labels = [];
    public newLabelName = "";

    public dataChartOptions = {
        chart: {
            type: 'column',
            width: null
        },

        boost: {
            useGPUTranslations: true
        },

        title: {
            useHTML: true,
            text: 'Loading'
        },

        subtitle: {
            text: ''
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

        series: [{
            name: 'Confidence',
            data: []
        }]
    };

    public sensors = [];

    constructor(
        private service: Service,
        private route: ActivatedRoute
    ) { }

    ngOnInit() {
        var that = this;
        setInterval(function() {
            that.service.fetchSensors().then(
                data => {
                    that.sensors = data['Data'];
                    that.loading = false;
                }
            );
        }, 1000);
    }

    public timers = [];

    public saveInstance(chartInstance, sensor) {
        var that = this;
        this.timers.push(setInterval(function() {
            that.service.fetchSensorAccumulatedData(sensor).then(
                data => {
                    var labels = [];
                    var values = [];
                    var lastUpdated = "";
                    for(let label in data['Data'])
                    {
                        labels.push(label);
                        values.push(data['Data'][label]);
                    }
                    that.service.fetchSensorLabel(sensor).then(
                        labelResponse => {
                            chartInstance.setTitle({text: sensor + ' - ' + labelResponse['Data']});
                        }
                    )
                    chartInstance.xAxis[0].categories = labels;
                    chartInstance.series[0].setData(values);
                }
            );
        }, 1000));
    }

    public reset() {
        for(let i = 0; i < this.timers.length; i++)
        {
            clearInterval(this.timers[i]);
        }
        this.service.resetSensorData().then();
    }

    public changeExistingLabel(id, label) {
        this.service.changeLabel(id, label).then(
            data => {
                if(data['Success']) {
                    this.labels = data['Data'];
                    this.newLabelName = "";
                }
            }
        )
    }

    public checkEnter(event, sensor) {
        console.log("sensorId: " + sensor);
        if((event as KeyboardEvent).key == "Enter"){
            this.changeExistingLabel(sensor, event.target.value);
        }
    }

    private paramsSubscription;
    public loading = true;

    private refreshChartData(){
        
    }
}