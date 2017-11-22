import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { RouterModule, Routes } from '@angular/router';
import { HttpClientModule } from '@angular/common/http';
import { RouterOutlet } from '@angular/router';
import { FormsModule }   from '@angular/forms';

import { ChartModule } from 'angular2-highcharts';
import { HighchartsStatic } from 'angular2-highcharts/dist/HighchartsService';
import * as highcharts from 'highcharts';

import { Service } from '../services/service';
import { AppComponent } from './app.component';
import { DataSet } from '../pages/dataset/dataset';
import { DataList } from '../pages/datalist/datalist';
import { Models } from '../pages/models/models';
import { Label } from '../pages/label/label';
import { NewDataSet } from '../pages/newdataset/newdataset';
import { NewModel } from '../pages/newmodel/newmodel';
import { NewModelType } from '../pages/newmodeltype/newmodeltype';
import { Test } from '../pages/test/test';
import { NewTest } from '../pages/newtest/newtest';
import { TestResult } from '../pages/testresult/testresult';

export const routes: Routes = [
  { path: 'dataset/:id', component: DataSet },
  { path: '', component: DataList },
  { path: 'data', component: DataList },
  { path: 'label', component: Label },
  { path: 'model', component: Models },
  { path: 'new', component: NewDataSet },
  { path: 'newmodel', component: NewModel },
  { path: 'newmodel/:id', component: NewModel },
  { path: 'newmodeltype', component: NewModelType },
  { path: 'newmodeltype/:id', component: NewModelType },
  { path: 'test', component: Test },
  { path: 'test/:id', component: TestResult },
  { path: 'newtest', component: NewTest }
];

declare var require: any;

export function highchartsFactory() {
  const hc = require('highcharts');
  const dd = require('highcharts/modules/boost');
  dd(hc);

  return hc;
}

@NgModule({
  declarations: [
    AppComponent,
    DataSet,
    Label,
    Models,
    DataList,
    NewDataSet,
    NewModel,
    NewModelType,
    Test,
    TestResult,
    NewTest
  ],
  imports: [
    BrowserModule,
    HttpClientModule,
    FormsModule,
    RouterModule.forRoot(
      routes
    ),
    ChartModule
  ],
  providers: [
    Service,
    {
      provide: HighchartsStatic,
      useFactory: highchartsFactory
    }
  ],
  bootstrap: [AppComponent]
})

export class AppModule { 
  
 }