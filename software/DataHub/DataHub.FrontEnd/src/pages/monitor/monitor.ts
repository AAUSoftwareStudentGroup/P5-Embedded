import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute } from '@angular/router';
import { Label } from '../label/label';
import { startTimeRange } from '@angular/core/src/profile/wtf_impl';


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

    public syncronize(list1 : any[], list2 : any[], comparer : ((a:any, b:any) => boolean)) {
        for(let i = 0; i < list2.length; i++) {
            if(list1.filter((s) => comparer(s, list2[i])).length == 0) {
                list1.push(list2[i]);
            }
        }

        for(let i = 0; i < list1.length; i++){
            if(list2.filter((s) => comparer(s, list1[i])).length == 0){
                list1.splice(i, 1);
            }
        }
    }

    private sensorTimer = null;

    ngOnInit() {
        this.startSensorTimer();
    }

    private startSensorTimer() {
        var that = this;
        this.sensorTimer = setInterval(function() {
            if(that.resetSensors) {
                that.resetSensors = false;
                clearInterval(that.sensorTimer);
                that.service.resetSensorData().then(data => {that.startSensorTimer();});
            } else {
                that.service.fetchSensors().then(
                    data => {
                        that.syncronize(that.sensors, data['Data'], (a,b) => a.Mac == b.Mac);

                        for(let i = 0; i < Math.min(that.sensors.length, data['Data'].length); i++) {
                            that.syncronize(that.sensors[i].Labels, data['Data'][i].Labels, (a, b) => a == b);
                        }

                        that.loading = false;
                    }
                );
            }
        }, 1000);
    }

    public labelChanged(mac, label){
        this.changeExistingLabel(mac, label);
    }

    public timers = [];

    public sensorLabels = [];

    public saveInstance(chartInstance, sensor) {
        var that = this;
        this.timers.push(setInterval(function() {
            that.service.fetchSensorAccumulatedData(sensor.Mac).then(
                data => {
                    var labels = [];
                    var values = [];
                    var lastUpdated = "";
                    for(let label in data['Data'])
                    {
                        labels.push(label);
                        values.push(data['Data'][label]);
                    }
                    that.service.fetchSensorLabel(sensor.Mac).then(
                        labelResponse => {
                            sensor.CurrentLabel = labelResponse['Data'];
                            chartInstance.setTitle({text: sensor.Mac + ' - ' + labelResponse['Data']});
                        }
                    )
                    chartInstance.xAxis[0].categories = labels;
                    chartInstance.series[0].setData(values);
                }
            );
        }, 1000));
    }

    private resetSensors = false;

    public reset() {
        for(let i = 0; i < this.timers.length; i++)
        {
            clearInterval(this.timers[i]);
        }
        this.resetSensors = true;
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