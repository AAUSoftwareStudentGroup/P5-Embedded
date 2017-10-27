using System;
using Encog.Neural.Networks;
using Encog.Neural.Networks.Layers;
using Encog.Engine.Network.Activation;
using Encog.ML.Data;
using Encog.Neural.Networks.Training.Propagation.Resilient;
using Encog.ML.Train;
using Encog.ML.Data.Basic;
using Encog;
using System.Linq;
using System.Collections.Generic;
using Encog.Util.KMeans;
using System.IO;
using System.Net.Http;
using DataHub.Messages;
using DataHub.Grouping;

namespace DataHub.Client
{
    public abstract class DataHubClient
    {
        public string ModelTypeID { get; set; }
        HttpClient client = new HttpClient();

        public DataHubClient(string modelName)
        {
            ModelTypeID = modelName;
            client.BaseAddress = new Uri("http://p5datahub.azurewebsites.net");
        }
            
        private TestInfo GetTestInfo()
        {
            var tests = client.GetAsync("/api/modeltype/"+ModelTypeID+"/test");

            Console.WriteLine("Sending request");

            string json = tests.Result.Content.ReadAsStringAsync().Result;
            var response = Newtonsoft.Json.JsonConvert.DeserializeObject<Response<TestInfo>>(json);

            Console.WriteLine("Received request");
            if (response.Success)
                return response.Data;
            else
                return null;
        }

        private TrainData IdentifyData(TestInfo testInfo)
        {
            TrainData trainData = new TrainData();


            foreach (var label in testInfo.Labels)
            {
                trainData.Add(label.Id, new List<Messages.Group>());
            }


            Console.WriteLine("Identifying");

            foreach (var trainSet in testInfo.TrainingSet)
            {
                foreach (var group in trainSet.Data)
                {
                    foreach (var label in trainSet.Labels)
                    {
                        if (trainData.ContainsKey(label.Id) && group.Data.Length > 10)
                            trainData[label.Id].Add(group);
                    }
                }
            }

            Console.WriteLine("Identified");

            int min = trainData.Min(d => d.Value.Count);

            Console.WriteLine(min);

            foreach (var key in trainData.Keys.ToArray())
            {
                trainData[key] = trainData[key].Take(min).ToList();
            }

            return trainData;
        }

        public abstract TestResult Train(TestInfo testInfo, TrainData trainData);

        private Response<TestResult> SendTestResult(TestResult result, TestInfo testInfo)
        {
            var content = new StringContent(Newtonsoft.Json.JsonConvert.SerializeObject(result));
            content.Headers.ContentType = new System.Net.Http.Headers.MediaTypeHeaderValue("application/json");
            var post = client.PostAsync("api/test/" + testInfo.Id + "/result", content);

            var temp = post.Result.Content.ReadAsStringAsync().Result;
            Console.WriteLine(temp);
            return Newtonsoft.Json.JsonConvert.DeserializeObject<Response<TestResult>>(temp);
        }

        public void Execute()
        {
            TestInfo testInfo = GetTestInfo();
            if (testInfo == null)
                return;
            TrainData trainData = IdentifyData(testInfo);
            TestResult result = Train(testInfo, trainData);

            SendTestResult(result, testInfo);
        }
    }
}
