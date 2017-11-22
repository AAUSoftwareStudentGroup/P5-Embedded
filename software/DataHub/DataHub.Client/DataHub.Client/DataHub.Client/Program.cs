using DataHub.Client.NeuralNetwork;
using DataHub.Client.Randy;
using DataHub.Client.SVM;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataHub.Client
{
    class Program
    {
        public static void Main(string[] args)
        {
            DataHubClient[] clients = new DataHubClient[] { new NNClient()/*, new SVMClient() */};
            while (true)
            {
                foreach (var client in clients)
                {
                    client.Execute();
                }
            }
        }
    }
}
