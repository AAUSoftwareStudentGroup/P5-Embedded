using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace DataHub.Grouping
{
    public class ShotIdentifier : IGroupIdentifier
    {
        public Group[] Identify(DataPoint[] data)
        {
            bool measuring = false;
            int zeroStreak = 0;
            Stack<DataPoint> groupData = new Stack<DataPoint>();
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
                        groupData = new Stack<DataPoint>();
                        measuring = false;
                    }
                }
                else
                {
                    measuring = true;
                    zeroStreak = 0;
                }
            }
            return groups.ToArray();
        }
    }
}
