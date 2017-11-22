import { Component } from '@angular/core';
import { Service } from '../../services/service';
import { ActivatedRoute, Router } from '@angular/router';


@Component({
    templateUrl: './newdataset.html'
})
export class NewDataSet {
     constructor(
        private service: Service,
        private route: ActivatedRoute,
        private router: Router
    ) { }

    public datalist = [];

    ngOnInit() {
      
    }

    public uploading = false;

    public upload(fileInput: any) {
        console.log(fileInput); 
        if (fileInput.target.files && fileInput.target.files[0]) {
            let total = fileInput.target.files.length;
            let counter = 0;
            for(let i = 0; i < total; i++) {
                var file = fileInput.target.files[i];
                var formData = new FormData();
                formData.append("datafile", file);
                this.uploading = true;
                this.service.uploadDataSet(formData).then(
                    data => {
                        if(++counter >= total) {
                            this.uploading = false;
                        }
                        if(data['Success'])
                            this.datalist.push(data['Data']);
                    }
                );
            }
            fileInput.target.value = "";
        }
    }
}