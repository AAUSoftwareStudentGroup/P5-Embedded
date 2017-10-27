using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DataHub.Messages;
using Encog.ML.Data.Basic;
using Encog.ML.SVM;
using Encog.ML.SVM.Training;
using Encog.ML.Train;
using DataHub.Grouping;

namespace DataHub.Client.SVM
{
    public class SVMClient : DataHubClient
    {
        public SVMClient() : base("SVM Encog 0.3")
        {
        }

        private BasicMLDataSet ConvertData(TestInfo testInfo, TrainData trainData)
        {
            List<double[]> inputs = new List<double[]>();
            List<double[]> outputs = new List<double[]>();

            int i = 0;

            Console.WriteLine("Converting");

            foreach (var key in trainData.Keys)
            {
                foreach (var group in trainData[key])
                {
                    List<double> input = new List<double>();

                    foreach (var data in group.Data.Take(10))
                    {
                        input.Add(data.X / 10000);
                        input.Add(data.Y / 10000);
                        input.Add(data.Z / 10000);
                        input.Add(data.RX / 30000);
                    }

                    double[] output = new double[1];
                    output[0] = i;
                    inputs.Add(input.ToArray());
                    outputs.Add(output);
                }

                i++;
            }

            List<double> randomSequence = new List<double>();

            Random rnd = new Random();

            for (int w = 0; w < 10000; w++)
            {
                randomSequence.Add(rnd.NextDouble());
            }

            int p = 0;
            inputs = inputs.OrderBy(d => randomSequence[p++ % 10000]).ToList();
            p = 0;
            outputs = outputs.OrderBy(d => randomSequence[p++ % 10000]).ToList();

            Console.WriteLine("Converted");

            return new BasicMLDataSet(inputs.ToArray(), outputs.ToArray());
        }

        public override TestResult Train(TestInfo testInfo, TrainData trainData)
        {
            BasicMLDataSet dataset = ConvertData(testInfo, trainData);

            var model = new SupportVectorMachine(40, false);

            model.Params.C = 100;
            model.Params.gamma = 0.1;

            IMLTrain train = new SVMSearchTrain(model, dataset);

            Console.WriteLine("Training");

            int iterations = int.Parse(testInfo.Parameters.First(pa => pa.Name == "Train Iterations").Value);
            for (int i = 0; i < iterations; i++)
            {
                train.Iteration();
                Console.WriteLine(train.Error);
            }

            train.FinishTraining();

            Console.WriteLine(string.Join(" ", trainData.Keys.Select(k => testInfo.Labels.First(l => l.Id == k).Name + "(" + trainData.Where(t => t.Key == k).Sum(q => q.Value.Count) + ")")));

            List<DataSetResult> datasetResults = new List<DataSetResult>();

            foreach (var testSet in testInfo.TestSet)
            {
                double[] confidence = new double[testInfo.Labels.Length];
                int no = 0;
                foreach (var group in testSet.Data)
                {

                    if (group.Data.Length > 10)
                    {
                        List<double> input = new List<double>();
                        foreach (var d in group.Data.Take(10))
                        {
                            input.Add(d.X / 20000);
                            input.Add(d.Y / 20000);
                            input.Add(d.Z / 20000);
                            input.Add(d.RX / 30000);
                        }
                        var output = model.Compute(new BasicMLData(input.ToArray()));
                        //Console.WriteLine(testSet.Name + ": " + testInfo.Labels[(int)output[0]].Name);

                        confidence[(int)output[0]]++;
                        no++;
                    }
                }

                confidence = confidence.Select(c => c / no).ToArray();

                List<Classification> classifications = new List<Classification>();

                for (int e = 0; e < testInfo.Labels.Length; e++)
                {
                    classifications.Add(new Classification()
                    {
                        Confidence = confidence[e],
                        LabelId = testInfo.Labels[e].Id
                    });
                }

                datasetResults.Add(new DataSetResult()
                {
                    DataSetId = testSet.Id,
                    Classifications = classifications.ToArray()
                });
                Console.WriteLine(testSet.Name + ": " + string.Join(" ", confidence.Select(c => Math.Round(c * 100) / 100)));
            }

            return new TestResult()
            {
                ModelId = testInfo.ModelId,
                DataSetResults = datasetResults.ToArray()
            };
        }
    }
}
