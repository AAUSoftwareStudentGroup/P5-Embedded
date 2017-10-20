using System;
using System.Collections.Generic;
using System.Text;

namespace DataHub.Grouping
{
    public interface IGroupIdentifier
    {
        Group[] Identify(DataPoint[] data);
    }
}
