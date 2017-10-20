using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class TestDataSet
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public Data[] Data { get; set; }
    }
}