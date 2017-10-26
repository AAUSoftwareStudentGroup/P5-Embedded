using DataHub.Messages;
using DataHub.Models;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using System.Web;
using System.Web.Http;
using System.Web.Http.Cors;

namespace DataHub.Controllers
{
    [EnableCors(origins: "*", headers: "*", methods: "*")]
    public class TestController : ApiController
    {
        [HttpGet]
        [Route("api/test")]
        public Response<Messages.Test[]> GetAllTests()
        {
            return new Response<Messages.Test[]>() { Data = new Entities().Test.Where(t => t.IsActive == 1).Select(t => new Messages.Test()
            {
                Id = t.Id,
                LabelIds = t.TestLabel.Select(l => l.LabelId),
                TestSetIds = t.TestDataSet.Where(d => d.IsTestSet == 1).Select(d => d.DataSetId),
                TrainingSetIds = t.TestDataSet.Where(d => d.IsTraningSet == 1).Select(d => d.DataSetId),
                ResultIds = t.TestResult.Select(r => r.Id)
            }).ToArray() };
        }

        [HttpGet]
        [Route("api/test/{id}")]
        public Response<Messages.Test> GetTest(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.Test>() { ErrorCode = ErrorCode.InvalidId };

            return new Response<Messages.Test>()
            {
                Data = new Entities().Test.Where(t => t.Id == id).Select(t => new Messages.Test()
                {
                    Id = t.Id,
                    LabelIds = t.TestLabel.Select(l => l.LabelId),
                    TestSetIds = t.TestDataSet.Where(d => d.IsTestSet == 1).Select(d => d.DataSetId),
                    TrainingSetIds = t.TestDataSet.Where(d => d.IsTraningSet == 1).Select(d => d.DataSetId),
                    ResultIds = t.TestResult.Select(r => r.Id)
                }).First()
            };
        }

        [HttpPost]
        [Route("api/test")]
        public Response<Messages.Test> AddTest(NewTest newTest)
        {
            var test = new Models.Test()
            {
                IsActive = 1,
                TestLabel = newTest.LabelIds.Select(l => new Models.TestLabel()
                {
                    LabelId = l
                }).ToArray(),
                TestDataSet = newTest.TestDataSetIds.Select(d => new Models.TestDataSet()
                {
                    IsTestSet = 1,
                    IsTraningSet = 0,
                    DataSetId = d
                }).Union(newTest.TrainingDataSetIds.Select(d => new Models.TestDataSet()
                {
                    IsTestSet = 0,
                    IsTraningSet = 1,
                    DataSetId = d
                })).ToArray()
            };

            using (Entities db = new Entities())
            {
                var added = db.Test.Add(test);

                db.SaveChanges();

                return GetTest(added.Id);
            }
        }

        [HttpPost]
        [Route("api/test/{id}/result")]
        public Response<Messages.TestResult> AddTestResult(int? id, Messages.TestResult result)
        {
            if (id == null)
                return new Response<Messages.TestResult>() { ErrorCode = ErrorCode.InvalidId };
            using (Entities db = new Entities())
            {
                var test = db.Test.FirstOrDefault(t => t.Id == id);
                if (test == null)
                    return new Response<Messages.TestResult>() { ErrorCode = ErrorCode.InvalidId };

                List<Models.Classification> classifications = new List<Models.Classification>();

                foreach (var datasetResult in result.DataSetResults)
                {
                    foreach (var classification in datasetResult.Classifications)
                    {
                        classifications.Add(new Models.Classification()
                        {
                            DataSetId = datasetResult.DataSetId,
                            LabelId = classification.LabelId,
                            Confidence = (float)classification.Confidence
                        });
                    }
                }

                var added = db.TestResult.Add(new Models.TestResult()
                {
                    ModelId = result.ModelId,
                    TestId = test.Id,
                    CreatedDate = DateTime.Now,
                    Classification = classifications
                });

                db.SaveChanges();

                result.Id = added.Id;

                return new Response<Messages.TestResult>() { Data = result };
            }
        }

        [HttpGet]
        [Route("api/test/{id}/result")]
        public Response<Messages.TestResult[]> GetTestResults(int? id)
        {
            if (id == null)
                return new Response<Messages.TestResult[]>() { ErrorCode = ErrorCode.InvalidId };
            using (Entities db = new Entities())
            {
                var test = db.Test.FirstOrDefault(t => t.Id == id);
                if (test == null)
                    return new Response<Messages.TestResult[]>() { ErrorCode = ErrorCode.InvalidId };

                return new Response<Messages.TestResult[]>()
                {
                    Data = test.TestResult.Select(t => new Messages.TestResult()
                    {
                        Id = t.Id,
                        ModelId = t.ModelId,
                        ModelName = t.Model.Name,
                        ModelTypeId = t.Model.ModelTypeId,
                        ModelTypeName = t.Model.ModelType.Name,
                        DataSetResults = t.Classification.GroupBy(c => c.DataSetId).Select(g => new Messages.DataSetResult()
                        {
                            LabelIds = g.First().DataSet.Mapping.Select(m => m.LabelId).ToArray(),
                            DataSetName = g.First().DataSet.Name,
                            DataSetId = g.Key,
                            Classifications = g.Select(c => new Messages.Classification()
                            {
                                LabelName = c.Label.Name,
                                Confidence = c.Confidence,
                                LabelId = c.LabelId
                            }).ToArray()
                        }).ToArray()
                    }).ToArray()
                };
            }
        }

        [HttpPost]
        [Route("api/test/{testId}/result/{testResultId}/modelfile")]
        public async Task<Response> UploadModelFile(int? testId, int? testResultId)
        {
            if (!Request.Content.IsMimeMultipartContent())
            {
                return new ErrorResponse() { ErrorCode = ErrorCode.MissingModelFile };
            }

            try
            {
                string root = HttpContext.Current.Server.MapPath("~/App_Data/ModelFiles");
                var provider = new MultipartFormDataStreamProvider(root);

                // Read the form data.
                await Request.Content.ReadAsMultipartAsync(provider);

                var file = provider.FileData.FirstOrDefault(f => f.Headers.ContentDisposition.DispositionType.Replace("\"", "") == "form-data" && f.Headers.ContentDisposition.Name.Replace("\"", "").ToLower() == "modelfile");

                if (file == null)
                {
                    return new Response() { ErrorCode = ErrorCode.MissingModelFile };
                }

                string fileName = Path.GetFileName(file.Headers.ContentDisposition.FileName.Replace("\"", ""));

                string mimeType = fileName.Split('.').LastOrDefault();

                if (mimeType == null || mimeType != "eg")
                {
                    return new ErrorResponse() { ErrorCode = ErrorCode.WrongFileType };
                }

                File.Copy(file.LocalFileName, $"{file.LocalFileName}.{mimeType}");

                File.Delete(file.LocalFileName);

                using (Entities db = new Entities())
                {
                    var result = db.TestResult.FirstOrDefault(r => r.Id == testResultId);

                    if (result == null)
                        return new ErrorResponse() { ErrorCode = ErrorCode.InvalidId };

                    result.ModelFileName = $"{file.LocalFileName}.{mimeType}";

                    db.SaveChanges();
                }

                return new Response();
            }
            catch (Exception e)
            {
                return new ErrorResponse() { ErrorCode = ErrorCode.CouldNotReadFile };
            }
        }

        [HttpGet]
        [Route("api/test/{testId}/result/{testResultId}/modelfile")]
        public Response GetModelFile(int? testId, int? testResultId)
        {
            using (Entities db = new Entities())
            {
                var result = db.TestResult.FirstOrDefault(r => r.Id == testResultId);

                if (result == null)
                    return new ErrorResponse() { ErrorCode = ErrorCode.InvalidId };

                string attachment = $"attachment; filename=modelfile";
                HttpContext.Current.Response.Clear();
                HttpContext.Current.Response.ClearHeaders();
                HttpContext.Current.Response.ClearContent();
                HttpContext.Current.Response.AddHeader("content-disposition", attachment);
                HttpContext.Current.Response.ContentType = "text/plain";
                HttpContext.Current.Response.Charset = "utf-8";
                HttpContext.Current.Response.BinaryWrite(File.ReadAllBytes(result.ModelFileName));
                HttpContext.Current.Response.End();
            }

            return new Response();
        }

        [HttpGet]
        [Route("api/modeltype/{id}/test")]
        public Response<TestInfo> GetTestForModelType(string id)
        {
            if (id == null)
                return new ErrorResponse<TestInfo>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var modelType = db.ModelType.Where(m => m.IsActive == 1).FirstOrDefault(m => m.Id.ToString() == id || m.Name == id);
                if (modelType == null)
                    return new ErrorResponse<TestInfo>() { ErrorCode = ErrorCode.InvalidId };

                var model = modelType.Model.Where(m => m.IsActive == 1).OrderBy(t => t.TestResult.Count).FirstOrDefault();

                if (model == null)
                    return new ErrorResponse<TestInfo>() { ErrorCode = ErrorCode.NoTestsAvailable };

                var test = db.Test.Where(t => t.IsActive == 1).OrderBy(t => t.TestResult.Where(r => r.ModelId == model.Id).Count()).FirstOrDefault();

                if (test == null)
                    return new Response<TestInfo>() { ErrorCode = ErrorCode.NoTestsAvailable };

                return new Response<TestInfo>()
                {
                    Data = new TestInfo()
                    {
                        Id = test.Id,
                        ModelId = model.Id,
                        ModelName = model.Name,
                        ModelTypeName = model.ModelType.Name,
                        ModelTypeId = model.ModelTypeId,
                        Labels = test.TestLabel.Select(t => new Messages.Label() { Id = t.LabelId, Name = t.Label.Name }).ToArray(),
                        Parameters = model.Parameter.Select(p => new Messages.Parameter()
                        {
                            Id = p.Id,
                            Name = p.Property.Name,
                            PropertyId = p.PropertyId,
                            Value = p.Value
                        }).ToArray(),
                        TestSet = test.TestDataSet.Where(t => t.IsTestSet == 1).Select(t => new Messages.TestDataSet()
                        {
                            Id = t.DataSetId,
                            Name = t.DataSet.Name,
                            Data = new DataSetController().GetDataByDataSetId(t.DataSetId).Data.ToArray(),
                        }).ToArray(),
                        TrainingSet = test.TestDataSet.Where(t => t.IsTraningSet == 1).Select(t => new Messages.DataLabelSet()
                        {
                            Id = t.DataSetId,
                            Data = new DataSetController().GetDataByDataSetId(t.DataSetId).Data.ToArray(),
                            Labels = t.DataSet.Mapping.Select(m => new Messages.Label()
                            {
                                Id = m.LabelId,
                                Name = m.Label.Name
                            }).ToArray()
                        }).ToArray()
                    }
                };
            }
        }

        [HttpDelete]
        [Route("api/test/{id}")]
        public Response DeleteTest(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.Test>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var test = db.Test.FirstOrDefault(t => t.Id == id);

                if (test == null)
                    return new ErrorResponse<Messages.Test>() { ErrorCode = ErrorCode.TestNotFound };

                test.IsActive = 0;

                db.SaveChanges();

                return GetTest(id);
            }
        }
    }
}
