using DataHub.Grouping;
using DataHub.Messages;
using Encog.Engine.Network.Activation;
using Encog.ML.Data.Basic;
using Encog.ML.Train;
using Encog.Neural.Networks;
using Encog.Neural.Networks.Layers;
using Encog.Neural.Networks.Training.Propagation.Resilient;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


namespace DataHub.Client.NeuralNetwork
{
    public class NNClient : DataHubClient
    {
        public NNClient() : base("Neural Network 1.1")
        {   }

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

        public override TestResult Train(TestInfo testInfo, TrainData trainData)
        {
            BasicMLDataSet dataset = ConvertData(testInfo, trainData);

            Dictionary<string, IActivationFunction> activationFunctionDict = new Dictionary<string, IActivationFunction>()
            {
                { "sigmoid",  new ActivationSigmoid() },
                { "relu", new ActivationReLU() },
                { "sofmax", new ActivationSoftMax() }
            };

            string[] activationFunctionInput = testInfo.Parameters.First(pa => pa.Name == "Activation Function").Value.Split(',');
            string[] biasNodeInput = testInfo.Parameters.First(pa => pa.Name == "Bias Node").Value.Split(',');
            string[] neuronsInput = testInfo.Parameters.First(pa => pa.Name == "Neurons").Value.Split(',');

            // intialise network and create first layer
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
            network.AddLayer(new BasicLayer(new ActivationSigmoid(), false, testInfo.Labels.Length));
            network.Structure.FinalizeStructure();
            network.Reset();

            IMLTrain train = new ResilientPropagation(network, dataset);

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
                            input.Add(d.X / 2000);
                            input.Add(d.Y / 2000);
                            input.Add(d.Z / 2000);
                            input.Add(d.RX / 10000);
                        }
                        var output = network.Compute(new BasicMLData(input.ToArray()));
                        for (int n = 0; n < output.Count; n++)
                        {
                            confidence[n] += output[n];
                        }
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
