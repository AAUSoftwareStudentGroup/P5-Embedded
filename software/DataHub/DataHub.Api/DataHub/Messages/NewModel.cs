using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class NewModel
    {
        public string Name { get; set; }
        public int TypeId { get; set; }
        public Parameter[] Parameters { get; set; }
    }
}