using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class DataLabelSet
    {
        public int Id { get; set; }
        public Data[] Data { get; set; }
        public Label[] Labels { get; set; }
    }
}