using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class Model
    {
        public int Id { get; set; }
        public string Name { get; set; }
        public string TypeName { get; set; }
        public int TypeId { get; set; }
        public Messages.Parameter[] Parameters { get; set; }
    }
}