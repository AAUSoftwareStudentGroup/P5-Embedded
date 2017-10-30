using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public class Response
    {
        public Response()
        {
            Success = true;
            ErrorCode = ErrorCode.NoError;
        }

        public bool Success { get; set; }
        public ErrorCode ErrorCode { get; set; }
        public string ErrorKey
        {
            get
            {
                return ErrorCode.ToString();
            }
        }
    }

    public class Response<TData> : Response
    {
        public TData Data { get; set; }
    }
}