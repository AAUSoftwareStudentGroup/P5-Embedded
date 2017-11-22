using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class Classification
    {
        public string LabelName { get; set; }
        public int LabelId { get; set; }
        public double Confidence { get; set; }
    }
}