using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace DataHub.Grouping
{
    public class DataParser
    {
        public DataPoint[] ParseCsv(string path)
        {
            List<DataPoint> data = new List<DataPoint>();
            foreach (var line in File.ReadAllLines(path))
            {
                var split = line.Split(';').ToArray();
                if (double.TryParse(split[0], out double t)
                    && double.TryParse(split[1], out double x)
                    && double.TryParse(split[2], out double y)
                    && double.TryParse(split[3], out double z)
                    && double.TryParse(split[4], out double rx)
                    && double.TryParse(split[5], out double ry)
                    && double.TryParse(split[6], out double rz))
                {
                    data.Add(new DataPoint() { Time = t, X = x, Y = y, Z = z, RX = rx, RY = ry, RZ = rz });
                }
            }
            return data.ToArray();
        }
    }
}
