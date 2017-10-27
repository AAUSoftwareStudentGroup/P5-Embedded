using DataHub.Messages;
using DataHub.Models;
using System;
using System.Collections.Generic;
using System.Data;
using System.IO;
using System.Linq;
using System.Linq.Expressions;
using System.Net;
using System.Net.Http;
using System.Threading.Tasks;
using System.Web;
using System.Web.Http;
using System.Web.Http.Cors;

namespace DataHub.Controllers
{
    [EnableCors(origins: "*", headers: "*", methods: "*")]
    public class DataSetController : ApiController
    {
        [HttpPost]
        [Route("api/dataset")]
        public async Task<Response<Messages.DataSet>> UploadDataFile()
        {
            if (!Request.Content.IsMimeMultipartContent())
            {
                return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.MissingDataFile };
            }

            try
            {
                string root = HttpContext.Current.Server.MapPath("~/App_Data/DataFiles");
                var provider = new MultipartFormDataStreamProvider(root);

                // Read the form data.
                await Request.Content.ReadAsMultipartAsync(provider);

                var file = provider.FileData.FirstOrDefault(f => f.Headers.ContentDisposition.DispositionType.Replace("\"", "") == "form-data" && f.Headers.ContentDisposition.Name.Replace("\"", "").ToLower() == "datafile");

                if (file == null)
                {
                    return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.MissingDataFile };
                }

                string fileName = Path.GetFileName(file.Headers.ContentDisposition.FileName.Replace("\"", "").Replace("CSV", "csv"));

                string mimeType = fileName.Split('.').LastOrDefault();

                if (mimeType == null && mimeType != "csv")
                {
                    return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.WrongFileType };
                }

                File.Copy(file.LocalFileName, $"{file.LocalFileName}.{mimeType}");

                File.Delete(file.LocalFileName);

                Models.DataSet dataSet = new Models.DataSet()
                {
                    Name = fileName,
                    LocalFileName = $"{file.LocalFileName}.{mimeType}",
                    MeasuredDate = DateTime.Now,
                    UploadedDate = DateTime.Now
                };

                Models.DataSet newDataSet;

                using (Entities db = new Entities())
                {
                    newDataSet = db.DataSet.Add(dataSet);
                    db.SaveChanges();
                }

                Messages.DataSet message = new Messages.DataSet()
                {
                    Id = newDataSet.Id,
                    Name = newDataSet.Name,
                    MeasuredDate = newDataSet.MeasuredDate,
                    UploadedDate = newDataSet.UploadedDate,
                    LabelIds = dataSet.Mapping.Select(m => m.LabelId).ToArray()
                };

                return new Response<Messages.DataSet>() { Data = message, ErrorCode = ErrorCode.NoError };
            }
            catch (Exception)
            {
                return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.CouldNotReadFile };
            }
        }

        [HttpGet]
        [Route("api/dataset/{id}")]
        public Response<Messages.DataSet> GetDataSetById(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var dataSet = db.DataSet.FirstOrDefault(d => d.Id == id);

                if (dataSet == null)
                    return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.DataSetNotFound };

                return new Response<Messages.DataSet>()
                {
                    Data = new Messages.DataSet()
                    {
                        Id = dataSet.Id,
                        Name = dataSet.Name,
                        MeasuredDate = dataSet.MeasuredDate,
                        UploadedDate = dataSet.UploadedDate,
                        LabelIds = dataSet.Mapping.Select(m => m.LabelId).ToArray()
                    },
                };
            }
        }

        [HttpDelete]
        [Route("api/dataset/{id}")]
        public Response DeleteDataSetById(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var dataSet = db.DataSet.FirstOrDefault(d => d.Id == id);

                if (dataSet == null)
                    return new ErrorResponse<Messages.DataSet>() { ErrorCode = ErrorCode.DataSetNotFound };

                if (dataSet.Classification.Count > 0 || dataSet.TestDataSet.Count > 0)
                    return new ErrorResponse<Messages.DataSet> { ErrorCode = ErrorCode.DataSetInUse };

                var mappings = dataSet.Mapping.ToArray();

                foreach (var mapping in mappings)
                {
                    db.Mapping.Remove(mapping);
                }

                db.DataSet.Remove(dataSet);
                db.SaveChanges();

                var labels = dataSet.Mapping.Select(m => m.Label).ToArray();

                return new Response<Messages.DataSet>()
                {
                    Data = new Messages.DataSet()
                    {
                        Id = dataSet.Id,
                        Name = dataSet.Name,
                        MeasuredDate = dataSet.MeasuredDate,
                        UploadedDate = dataSet.UploadedDate,
                        LabelIds = dataSet.Mapping.Select(m => m.LabelId).ToArray()
                    },
                };
            }
        }

        [HttpPost]
        [Route("api/dataset/{id}/label")]
        public Response<Messages.Label> AddLabelToDataSet(int? id, Messages.Label label)
        {
            if (id == null)
                return new ErrorResponse<Messages.Label>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var dataSet = db.DataSet.FirstOrDefault(d => d.Id == id);

                if (dataSet == null)
                    return new ErrorResponse<Messages.Label>() { ErrorCode = ErrorCode.DataSetNotFound };

                dataSet.Mapping.Add(new Mapping()
                {
                    LabelId = label.Id
                });

                db.SaveChanges();

                return new Response<Messages.Label>()
                {
                    Data = new Messages.Label()
                    {
                        Id = label.Id,
                        Name = label.Name
                    }
                };
            }
        }

        [HttpGet]
        [Route("api/label")]
        public Response<Messages.Label[]> GetLabels()
        {
            using (Entities db = new Entities())
            {
                return new Response<Messages.Label[]>()
                {
                    Data = db.Label.Select(l => new Messages.Label()
                    {
                        Id = l.Id,
                        Name = l.Name
                    }).ToArray()
                };
            }
        }

        [HttpDelete]
        [Route("api/dataset/{id}/label/{labelId}")]
        public Response<Messages.Label> DeleteLabelFromDataSet(int? id, int? labelId)
        {
            if (id == null || labelId == null)
                return new ErrorResponse<Messages.Label>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var dataSet = db.DataSet.FirstOrDefault(d => d.Id == id);

                if (dataSet == null)
                    return new ErrorResponse<Messages.Label>() { ErrorCode = ErrorCode.DataSetNotFound };

                var mapping = dataSet.Mapping.FirstOrDefault(m => m.LabelId == labelId);

                if (mapping == null)
                    return new ErrorResponse<Messages.Label>() { ErrorCode = ErrorCode.InvalidLabelId };

                var label = db.Label.First(l => l.Id == mapping.LabelId);

                if (dataSet.TestDataSet.Where(t => t.Test.TestLabel.Where(l => l.Id == label.Id).Count() > 0).Count() > 0)
                    return new ErrorResponse<Messages.Label>() { ErrorCode = ErrorCode.LabelUsedInTests };

                var map = db.Mapping.FirstOrDefault(m => m.Id == mapping.Id);

                db.Mapping.Remove(map);

                db.SaveChanges();

                return new Response<Messages.Label>()
                {
                    Data = new Messages.Label()
                    {
                        Id = label.Id,
                        Name = label.Name
                    }
                };
            }
        }

        [HttpGet]
        [Route("api/dataset/{id}/label")]
        public Response<Messages.Label[]> GetLabelsForDataSet(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.Label[]>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var dataSet = db.DataSet.FirstOrDefault(d => d.Id == id);

                if (dataSet == null)
                    return new ErrorResponse<Messages.Label[]>() { ErrorCode = ErrorCode.DataSetNotFound };

                var labels = dataSet.Mapping.Select(m => m.Label);

                return new Response<Messages.Label[]>()
                {
                    Data = labels.Select(l => new Messages.Label()
                    {
                        Id = l.Id,
                        Name = l.Name
                    }).ToArray()
                };
            }
        }

        [HttpPost]
        [Route("api/label")]
        public Response<Messages.Label> AddLabel(Messages.Label label)
        {
            using (Entities db = new Entities())
            {
                if (db.Label.FirstOrDefault(m => m.Name == label.Name) != null)
                    return new ErrorResponse<Messages.Label>() { ErrorCode = ErrorCode.NameAlreadyInUse };

                var added = db.Label.Add(new Models.Label()
                {
                    Name = label.Name
                });

                db.SaveChanges();

                return new Response<Messages.Label>()
                {
                    Data = new Messages.Label()
                    {
                        Id = added.Id,
                        Name = added.Name
                    }
                };
            }
        }

        [HttpDelete]
        [Route("api/label/{id}")]
        public Response<Messages.Label> DeleteLabel(int? id)
        {
            if (id == null)
                return new Response<Messages.Label>() { ErrorCode = ErrorCode.InvalidId };


            using (Entities db = new Entities())
            {
                var label = db.Label.FirstOrDefault(l => l.Id == id);

                if (label == null)
                    return new Response<Messages.Label>() { ErrorCode = ErrorCode.InvalidLabelId };

                var mappings = db.Mapping.Where(m => m.LabelId == id).ToList();
                foreach (var mapping in mappings)
                {
                    db.Mapping.Remove(mapping);
                }

                db.Label.Remove(label);

                db.SaveChanges();

                return new Response<Messages.Label>()
                {
                    Data = new Messages.Label()
                    {
                        Id = label.Id,
                        Name = label.Name
                    }
                };
            }
        }

        [HttpGet]
        [Route("api/dataset")]
        public Response<Messages.DataSet[]> GetAllDataSets()
        {
            using (Entities db = new Entities())
            {
                var dataSets = db.DataSet.ToArray();

                return new Response<Messages.DataSet[]>()
                {
                    Data = dataSets.Select(d => new Messages.DataSet()
                    {
                        Id = d.Id,
                        Name = d.Name,
                        MeasuredDate = d.MeasuredDate,
                        UploadedDate = d.UploadedDate,
                        LabelIds = d.Mapping.Select(m => m.LabelId).ToArray()
                    }).ToArray()
                };
            }
        }

        // ______________________________________________________________________________________________________________________________________________________________________________________________________
        //Opgave 1: mv Grouping from DH.Client to here, so GetDataByDataSetId returns List<Messages.Group> instead of Response<Messages.Data[]>. This needs the ShotIdentifier to group data. Findes herunder.
        //Opgave 2: create new Client to send random results back (a random number between 0-1 for each person it guesses). This requires changes in the client, since it has to return probabilities instead of data. Findes i RandomClient i DataHub.Client
        // ______________________________________________________________________________________________________________________________________________________________________________________________________

        //Opgave 3: currently data gets to the client through an API-call to the database. It is sent as TestInfo with all data in TrainData (methodType/{id}/test). Instead one should e.g. call (methodType/{id}/test?cached=LIST) which should send empty data results for the cached objects, so they are not downloaded unnecessarily. The objects should still exist, just with datavariable=null.
        // Found in TestController.cs
        // ______________________________________________________________________________________________________________________________________________________________________________________________________

        // OPGAVE: når der trænes, skal der sendes en fil med, hør morten.
        [HttpGet]
        [Route("api/dataset/{id}/data")]
        public Response<List<Group>> GetDataByDataSetId(int? id)
        {
            if (id == null)
                return new Response<List<Group>>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var dataSet = db.DataSet.FirstOrDefault(d => d.Id == id);

                if (dataSet == null)
                    return new Response<List<Group>>() { ErrorCode = ErrorCode.DataSetNotFound };

                try
                {
                    List<Data> data = new List<Data>();
                    double t, x, y, z, rx, ry, rz;
                    foreach (var line in File.ReadAllLines(dataSet.LocalFileName))
                    {
                        var split = line.Split(';').ToArray();
                        if (double.TryParse(split[0], out t)
                            && double.TryParse(split[1], out x)
                            && double.TryParse(split[2], out y)
                            && double.TryParse(split[3], out z)
                            && double.TryParse(split[4], out rx)
                            && double.TryParse(split[5], out ry)
                            && double.TryParse(split[6], out rz))
                        {
                            data.Add(new Data() { Time = t, X = x, Y = y, Z = z, RX = rx, RY = ry, RZ = rz });
                        }
                    }
                    List<Data> downScaled = new List<Data>();
                    t = 0; x = 0; y = 0; z = 0; rx = 0; ry = 0; rz = 0;
                    int c = 0;
                    int scale = 20;
                    foreach (var d in data)
                    {
                        t += d.Time;
                        x += d.X;
                        y += d.Y;
                        z += d.Z;
                        rx += d.RX;
                        ry += d.RY;
                        rz += d.RZ;
                        c++;
                        if (c == scale)
                        {
                            downScaled.Add(new Data() { Time = t / scale, X = x / scale, Y = y / scale, Z = z / scale, RX = rx / scale, RY = ry / scale, RZ = rz / scale });
                            t = 0; x = 0; y = 0; z = 0; rx = 0; ry = 0; rz = 0;
                            c = 0;
                        }
                    }

                    ShotIdentifier shotIdentifier = new ShotIdentifier();

                    return new Response<List<Group>>() { Data = shotIdentifier.Identify(data) };
                }
                catch (Exception)
                {
                    return new Response<List<Group>>() { ErrorCode = ErrorCode.CouldNotReadData };
                }
            }
        }
    }
}
