using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;

namespace DataHub.Controllers
{
    public class VersionController : ApiController
    {
        [HttpGet]
        public string GetVersion() => "0.0.1";
    }
}
