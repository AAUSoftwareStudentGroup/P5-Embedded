using DataHub.Client.NeuralNetwork;
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
            DataHubClient client = new NNClient();
            while (true)
            {
                client.Execute();
            }
        }
    }
}
