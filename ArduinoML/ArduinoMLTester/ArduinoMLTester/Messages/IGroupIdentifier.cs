using System;
using System.Collections.Generic;
using System.Text;

namespace DataHub.Messages
{
    public interface IGroupIdentifier
    {
        List<Group> Identify(List<Data> data);
    }
}
