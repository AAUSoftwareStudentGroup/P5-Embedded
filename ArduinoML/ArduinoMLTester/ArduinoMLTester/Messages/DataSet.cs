using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class DataSet
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public DateTime UploadedDate { get; set; }
        public DateTime MeasuredDate { get; set; }
        public int[] LabelIds { get; set; }
    }
}