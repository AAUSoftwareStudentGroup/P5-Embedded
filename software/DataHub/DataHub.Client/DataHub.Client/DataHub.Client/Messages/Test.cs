using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class Test
    {
        public int Id { get; set; }
        public IEnumerable<int> LabelIds { get; set; }
        public IEnumerable<int> TrainingSetIds { get; set; }
        public IEnumerable<int> TestSetIds { get; set; }
        public IEnumerable<int> ResultIds { get; set; }
        public double Accurracy { get; set; }
        public string Status { get; set; }
    }
}