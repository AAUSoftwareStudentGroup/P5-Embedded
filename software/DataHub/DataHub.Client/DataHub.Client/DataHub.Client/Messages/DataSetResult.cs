using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class DataSetResult
    {
        public string DataSetName { get; set; }
        public int DataSetId { get; set; }
        public int[] LabelIds { get; set; }
        public Classification[] Classifications { get; set; }
    }
}