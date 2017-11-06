using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class NewModelType
    {
        public string Name { get; set; }
        public Property[] Properties { get; set; }
    }
}