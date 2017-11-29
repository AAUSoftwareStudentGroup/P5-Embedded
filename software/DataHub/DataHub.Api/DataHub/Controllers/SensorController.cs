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

        public class SensorInfo
        {
            public string Label;
            public List<LiveData> LiveData = new List<SensorController.LiveData>();
        }

        public class Sensor
        {
            public string Mac;
            public string CurrentLabel;
            public string[] Labels;
        }

        //mac address -> classifications
        static Dictionary<string, SensorInfo> sensorInfo = new Dictionary<string, SensorInfo>();

        [HttpPost]
        [Route("api/sensor/{sensorId}/{label}")]
        public Response<string> SetLabel(string sensorId, string label)
        {
            if (sensorInfo.ContainsKey(sensorId))
            {
                sensorInfo[sensorId].Label = label;
            }
            else
                sensorInfo.Add(sensorId, new SensorInfo() { Label = label });

            return new Response<string>() { Data = sensorInfo[sensorId].Label };
        }

        [HttpGet]
        [Route("api/sensor/labels")]
        public string GetLabels()
        {
            string labelString = "";
            foreach (var item in sensorInfo)
            {
                labelString += $"{item.Key}#{item.Value.Label}\r\n";
            }

            return labelString;
        }

        [HttpPost]
        [Route("api/sensor/data")]
        public string AddLiveData([FromBody]string data)
        {
            if (data == null)
                return GetLabels();

            var lines = data.Replace("\r", "").Split('\n');
            foreach (var line in lines)
            {
                var live = new LiveData();
                live.Time = DateTime.Now;
                var lineSplit = line.Split('#');
                if (lineSplit.Length != 2)
                    return GetLabels();
                live.Mac = lineSplit[0].Replace(":","");
                var labelArray = lineSplit[1].Split(';');
                foreach (var label in labelArray)
                {
                    var labelSplit = label.Split(':');
                    if (labelSplit.Length != 2)
                        continue;
                    var name = labelSplit[0];
                    var confidence = double.Parse(labelSplit[1]);
                    live.Classifications.Add(name, confidence);
                }
                if (sensorInfo.ContainsKey(live.Mac))
                    sensorInfo[live.Mac].LiveData.Add(live);
                else
                {
                    sensorInfo.Add(live.Mac, new SensorInfo() { LiveData = new List<LiveData> { live }, Label = ""});
                }

            }
            return GetLabels();
        }

        [HttpGet]
        [Route("api/sensor")]
        public Response<List<Sensor>> GetSensors()
        {
            List<Sensor> sensors = new List<Sensor>();
            foreach (var item in sensorInfo)
            {
                Sensor sensor = new Sensor();
                sensor.Mac = item.Key;
                sensor.CurrentLabel = item.Value.Label;
                sensor.Labels = item.Value.LiveData.Last().Classifications.Keys.ToArray();
                sensors.Add(sensor);
            }
            return new Response<List<Sensor>>() { Data = sensors };
        }

        [HttpGet]
        [Route("api/sensor/{sensorId}/data")]
        public Response<LiveData[]> GetLiveData(string sensorId)
        {
            List<LiveData> response = new List<LiveData>();
            if (sensorInfo.Count > 0)
            {
                foreach (var item in sensorInfo[sensorId].LiveData)
                {
                    response.Add(item);
                }
            }
            return new Response<LiveData[]>() { Data = response.OrderBy(d => d.Time).ToArray() };
        }

        [HttpGet]
        [Route("api/sensor/{sensorId}/label")]
        public Response<string> GetSensorLabel(string sensorId)
        {
            if(sensorId == null || !sensorInfo.ContainsKey(sensorId))
            {
                return new ErrorResponse<string>();
            }

            return new Response<string>() { Data = sensorInfo[sensorId].Label };
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
            sensorInfo.Clear();
            return new Response();
        }
    }
}
