using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class ErrorResponse : Response
    {
        public ErrorResponse()
        {
            Success = false;
        }
    }

    public class ErrorResponse<TData> : Response<TData>
    {
        public ErrorResponse()
        {
            Success = false;
        }
    }
}