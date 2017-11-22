using DataHub.Messages;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Web.Http;
using System.Web.Http.Cors;

namespace DataHub.Controllers
{
    [EnableCors(origins: "*", headers: "*", methods: "*")]
    public class SensorController : ApiController
    {
        public class LiveData
        {
            public DateTime Time { get; set; }
            public string Mac { get; set; }
            public Dictionary<string, double> Classifications { get; set; } = new Dictionary<string, double>();
        }

        //mac address -> classifications
        static Dictionary<string, List<LiveData>> liveData = new Dictionary<string, List<LiveData>>();

        [HttpPost]
        [Route("api/sensor/data")]
        public bool AddLiveData([FromBody]string data)
        {
            var lines = data.Replace("\r", "").Split('\n');
            foreach (var line in lines)
            {
                var live = new LiveData();
                live.Time = DateTime.Now;
                var lineSplit = line.Split('#');
                if (lineSplit.Length != 2)
                    return false;
                live.Mac = lineSplit[0].Replace(":","");
                var labels = lineSplit[1].Split(';');
                foreach (var label in labels)
                {
                    var labelSplit = label.Split(':');
                    if (labelSplit.Length != 2)
                        continue;
                    var name = labelSplit[0];
                    var confidence = double.Parse(labelSplit[1]);
                    live.Classifications.Add(name, confidence);
                }
                if (liveData.ContainsKey(live.Mac))
                    liveData[live.Mac].Add(live);
                else
                    liveData.Add(live.Mac, new List<LiveData>() { live });
            }
            return true;
        }

        [HttpGet]
        [Route("api/sensor")]
        public Response<List<string>> GetSensors()
        {
            return new Response<List<string>>() { Data = liveData.Keys.ToList() };
        }

        [HttpGet]
        [Route("api/sensor/{sensorId}/data")]
        public Response<LiveData[]> GetLiveData(string sensorId)
        {
            List<LiveData> response = new List<LiveData>();
            if (liveData.Count > 0)
            {
                foreach (var item in liveData[sensorId])
                {
                    response.Add(item);
                }
            }
            return new Response<LiveData[]>() { Data = response.OrderBy(d => d.Time).ToArray() };
        }

        [HttpGet]
        [Route("api/sensor/{sensorId}/data/{windowSize}")]
        public Response<Dictionary<string, double>> GetAccumulatedLiveData(string sensorId, int? windowSize)
        {
            windowSize = Math.Max(1, windowSize ?? 50);
            Dictionary<string, List<double>> grouped = new Dictionary<string, List<double>>();

            var ordered = GetLiveData(sensorId).Data;

            foreach (var data in ordered.Reverse().Take(windowSize.Value).Reverse()) //Last 20 classifications
            {
                foreach (var classification in data.Classifications)
                {
                    if (grouped.ContainsKey(classification.Key))
                        grouped[classification.Key].Add(classification.Value);
                    else
                        grouped.Add(classification.Key, new List<double>() { classification.Value });
                }
            }

            Dictionary<string, double> response = new Dictionary<string, double>();

            foreach (var key in grouped.Keys)
            {
                if (grouped[key].Count > 0)
                    response.Add(key, grouped[key].Sum() / grouped[key].Count);
            }
            return new Response<Dictionary<string, double>>() { Data = response };
        }

        [HttpDelete]
        [Route("api/sensor/data")]
        public Response ResetData()
        {
            liveData.Clear();
            return new Response();
        }
    }
}
