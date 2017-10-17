import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import 'rxjs/add/operator/map';
import 'rxjs/add/operator/toPromise';

@Injectable()
export class Service {
  
    constructor(private http: HttpClient) { }
    
    private TEST = false;

    private API_URL = this.TEST ? "http://localhost:65230" : "https://p5datahub.azurewebsites.net";

    public fetchData(id) {
        return this.http.get(this.API_URL + "/api/dataset/" + id + "/data")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchDataSet(id) {
        return this.http.get(this.API_URL + "/api/dataset/" + id)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchDataSetLabels(id) {
        return this.http.get(this.API_URL + "/api/dataset/" + id + "/label")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public addDataSetLabel(id, label) {
        return this.http.post(this.API_URL + "/api/dataset/" + id + "/label", label)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public deleteDataSet(id) {
        return this.http.delete(this.API_URL + "/api/dataset/" + id)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public deleteLabel(id) {
        return this.http.delete(this.API_URL + "/api/label/" + id)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public deleteDataSetLabel(id, labelId) {
        return this.http.delete(this.API_URL + "/api/dataset/" + id + "/label/" + labelId)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public addLabel(label) {
        return this.http.post(this.API_URL + "/api/label", label)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchLabels() {
        return this.http.get(this.API_URL + "/api/label")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchDataList() {
        return this.http.get(this.API_URL + "/api/dataset")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public uploadDataSet(data) {
        return this.http.post(this.API_URL + "/api/dataset", data)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public addModelType(modelType) {
        return this.http.post(this.API_URL + "/api/modeltype", modelType)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public addModel(model) {
        return this.http.post(this.API_URL + "/api/model", model)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchModelTypes() {
        return this.http.get(this.API_URL + "/api/modeltype")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchModels() {
        return this.http.get(this.API_URL + "/api/model")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchModelTypeProperties(id) {
        return this.http.get(this.API_URL + "/api/modeltype/" + id + "/property")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchTests() {
        return this.http.get(this.API_URL + "/api/test")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public addTest(test) {
        return this.http.post(this.API_URL + "/api/test", test)
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }

    public fetchResults(id) {
        return this.http.get(this.API_URL + "/api/test/" + id + "/result")
            .toPromise()
            .catch(error => {
                console.debug('Error fetching accounts: ' + error.message);
            });
    }
}
