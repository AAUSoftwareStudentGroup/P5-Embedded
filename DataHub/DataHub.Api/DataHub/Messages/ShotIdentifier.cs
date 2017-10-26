using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace DataHub.Messages
{
    public class ShotIdentifier : IGroupIdentifier
    {
        public List<Group> Identify(List<Data> data)
        {
            bool measuring = false;
            int zeroStreak = 0;
            Stack<Data> groupData = new Stack<Data>();
            List<Group> groups = new List<Group>();
            foreach (var point in data)
            {
                var val = point.RX > 5000 ? 1 : (point.RX < -5000 ? -1 : 0);
                if (measuring)
                {
                    groupData.Push(point);
                }
                if (val == 0)
                {
                    zeroStreak++;
                    if (zeroStreak == 10)
                    {
                        groups.Add(new Group() { Data = groupData.Reverse().ToArray() });
                        groupData = new Stack<Data>();
                        measuring = false;
                    }
                }
                else
                {
                    measuring = true;
                    zeroStreak = 0;
                }
            }
            return groups;
        }
    }
}
