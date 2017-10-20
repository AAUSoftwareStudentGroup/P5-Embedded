using DataHub.Client.NeuralNetwork;
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
            NNClient nnClient = new NNClient();
            while (true)
            {
                nnClient.Execute();
            }
        }
    }
}
