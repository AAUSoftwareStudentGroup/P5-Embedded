using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class NewTest
    {
        public int[] LabelIds { get; set; }
        public int[] TrainingDataSetIds { get; set; }
        public int[] TestDataSetIds { get; set; }
    }
}