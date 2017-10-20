using System;
using System.Collections.Generic;
using System.Text;
using System.Linq;

namespace DataHub.Grouping
{
    public class DataSetFactory
    {
        public static DataSet FromCsvFiles(Mapping[] pathOutputMapping, Func<DataPoint, double[]> valueFunc, int minInputSize)
        {
            List<double[]> inputSet = new List<double[]>();
            List<double[]> outputSet = new List<double[]>();

            var parser = new DataParser();
            var identifier = new ShotIdentifier();

            foreach (var mapping in pathOutputMapping)
            {
                var data = parser.ParseCsv(mapping.Path);
                foreach (var group in identifier.Identify(data))
                {
                    if (group.Data.Length >= minInputSize)
                    {
                        IEnumerable<double> union = new List<double>();
                        var mapped = group.Data.Take(minInputSize).Select(d => union = union.Union(valueFunc(d))).ToArray();
                        inputSet.Add(union.ToArray());
                        outputSet.Add(mapping.Output);
                    }
                }
            }

            return new DataSet() { InputSet = inputSet.ToArray(), OutputSet = outputSet.ToArray() };
        }
    }
}
