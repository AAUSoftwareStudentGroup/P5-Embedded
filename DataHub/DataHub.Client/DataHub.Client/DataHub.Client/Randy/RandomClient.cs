using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DataHub.Messages;

namespace DataHub.Client.Randy
{
    class RandomClient : DataHubClient
    {
        public RandomClient() : base("Random 1.0")
        {    }

        public override TestResult Train(TestInfo testInfo, TrainData trainData)
        {
            double[] confidence = new double[testInfo.Labels.Length];
            List<DataSetResult> datasetResults = new List<DataSetResult>();
            Random random = new Random();

            foreach (var testSet in testInfo.TestSet)
            {
                List<Classification> classifications = new List<Classification>();

                for (int e = 0; e < testInfo.Labels.Length; e++)
                {
                    classifications.Add(new Classification()
                    {
                        LabelId = testInfo.Labels[e].Id,
                        Confidence = random.NextDouble()
                    });
                }

                datasetResults.Add(new DataSetResult()
                {
                    DataSetId = testSet.Id,
                    Classifications = classifications.ToArray()
                });
            }

            return new TestResult()
            {
                ModelId = testInfo.ModelId,
                ModelTypeId = testInfo.ModelTypeId,
                DataSetResults = datasetResults.ToArray()
            };
        }
    }
}
