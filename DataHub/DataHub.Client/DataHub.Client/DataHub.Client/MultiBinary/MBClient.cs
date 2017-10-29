using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using DataHub.Messages;
using Encog.Engine.Network.Activation;
using Encog.Neural.Networks.Layers;
using Encog.Neural.Networks;
using Encog.ML.Data.Basic;
using Encog.Neural.Networks.Training.Propagation.Resilient;
using Encog.ML.Train;

namespace DataHub.Client.MultiBinary
{
    class MBClient : DataHubClient
    {
        public MBClient() : base("Multi Binary 1.2")
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
                        input.Add(data.X / 2000);
                        input.Add(data.Y / 2000);
                        input.Add(data.Z / 2000);
                        input.Add(data.RX / 10000);
                    }

                    double[] output = new double[trainData.Keys.Count];
                    output[i] = 1;
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

        private BasicMLDataSet GetDataSet(int labelId, int otherLabelId, TestInfo testInfo, TrainData trainData)
        {
            List<double[]> inputs = new List<double[]>();
            List<double[]> outputs = new List<double[]>();

            int i = 0;

            Console.WriteLine("Converting");

            foreach (var group in trainData[labelId])
            {
                List<double> input = new List<double>();

                foreach (var data in group.Data.Take(10))
                {
                    input.Add(data.X / 2000);
                    input.Add(data.Y / 2000);
                    input.Add(data.Z / 2000);
                    input.Add(data.RX / 10000);
                }

                double[] output = new double[2];
                output[0] = 1;
                inputs.Add(input.ToArray());
                outputs.Add(output);
            }

            foreach (var group in trainData[otherLabelId])
            {
                List<double> input = new List<double>();

                foreach (var data in group.Data.Take(10))
                {
                    input.Add(data.X / 2000);
                    input.Add(data.Y / 2000);
                    input.Add(data.Z / 2000);
                    input.Add(data.RX / 10000);
                }

                double[] output = new double[2];
                output[1] = 1;
                inputs.Add(input.ToArray());
                outputs.Add(output);
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

            Dictionary<string, IActivationFunction> activationFunctionDict = new Dictionary<string, IActivationFunction>()
            {
                { "sigmoid",  new ActivationSigmoid() },
                { "relu", new ActivationReLU() },
                { "sofmax", new ActivationSoftMax() }
            };

            string[] activationFunctionInput = testInfo.Parameters.First(pa => pa.Name == "Activation Function").Value.Split(',');
            string[] biasNodeInput = testInfo.Parameters.First(pa => pa.Name == "Bias Node").Value.Split(',');
            string[] neuronsInput = testInfo.Parameters.First(pa => pa.Name == "Neurons").Value.Split(',');

            Dictionary<int, Dictionary<int, BasicNetwork>> labelNetworks = new Dictionary<int, Dictionary<int, BasicNetwork>>();

            foreach (var label in testInfo.Labels)
            {
                Dictionary<int, BasicNetwork> networks = new Dictionary<int, BasicNetwork>();
                foreach (var otherLabel in testInfo.Labels)
                {
                    if(otherLabel != label)
                    {
                        var network = new BasicNetwork();
                        network.AddLayer(new BasicLayer(null, true, 40));

                        // assuming the four arrays are of same length, perhaps make a check for this
                        // add layers corresponding to parameter input
                        for (int i = 0; i < biasNodeInput.Length; i++)
                        {
                            var activationFunction = activationFunctionDict[activationFunctionInput[i].Trim()];
                            var hasBias = biasNodeInput[i].Trim() == "true";
                            var neuronCount = int.Parse(neuronsInput[i]);

                            network.AddLayer(new BasicLayer(activationFunction, hasBias, neuronCount));
                        }

                        // create last layer and finalise structure of network
                        network.AddLayer(new BasicLayer(new ActivationSigmoid(), false, 2));
                        network.Structure.FinalizeStructure();
                        network.Reset();

                        BasicMLDataSet dataset = GetDataSet(label.Id, otherLabel.Id, testInfo, trainData);

                        IMLTrain train = new ResilientPropagation(network, dataset);

                        Console.WriteLine("Training");

                        int iterations = int.Parse(testInfo.Parameters.First(pa => pa.Name == "Train Iterations").Value);
                        for (int i = 0; i < iterations; i++)
                        {
                            train.Iteration();
                            Console.WriteLine(train.Error);
                        }

                        train.FinishTraining();

                        networks.Add(otherLabel.Id, network);
                    }
                }
                labelNetworks.Add(label.Id, networks);
            }

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
                            input.Add(d.X / 2000);
                            input.Add(d.Y / 2000);
                            input.Add(d.Z / 2000);
                            input.Add(d.RX / 10000);
                        }
                        int i = 0;
                        foreach (var label in testInfo.Labels)
                        {
                            foreach (var network in labelNetworks[label.Id])
                            {
                                var output = network.Value.Compute(new BasicMLData(input.ToArray()));
                                confidence[i] += output[0];
                            }
                            i++;
                        }
                        no += testInfo.Labels.Count() - 1;
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
