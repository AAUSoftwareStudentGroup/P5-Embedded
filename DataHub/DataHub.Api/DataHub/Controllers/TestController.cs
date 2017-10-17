﻿using DataHub.Messages;
using DataHub.Models;
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
    public class TestController : ApiController
    {
        [HttpGet]
        [Route("api/test")]
        public Response<Messages.Test[]> GetAllTests()
        {
            return new Response<Messages.Test[]>() { Data = new Entities().Test.Select(t => new Messages.Test()
            {
                Id = t.Id,
                ModelName = t.Model.Name,
                ModelTypeName = t.Model.ModelType.Name,
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
                    ModelName = t.Model.Name,
                    ModelTypeName = t.Model.ModelType.Name,
                    LabelIds = t.TestLabel.Select(l => l.LabelId),
                    TestSetIds = t.TestDataSet.Where(d => d.IsTestSet == 1).Select(d => d.DataSetId),
                    TrainingSetIds = t.TestDataSet.Where(d => d.IsTraningSet == 1).Select(d => d.DataSetId),
                    ResultIds = t.TestResult.Select(r => r.Id)
                }).First()
            };
        }

        [HttpPost]
        [Route("api/test")]
        public Response<Messages.Test> AddTest(Messages.NewTest newTest)
        {
            var test = new Models.Test()
            {
                ModelId = newTest.ModelId,
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
                        ModelName = t.Test.Model.Name,
                        ModelTypeName = t.Test.Model.ModelType.Name,
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

        [HttpGet]
        [Route("api/modeltype/{id}/test")]
        public Response<TestInfo> GetTestForModelType(int? id)
        {
            if (id == null)
                return new ErrorResponse<TestInfo>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var modelType = db.ModelType.FirstOrDefault(m => m.Id == id);
                if (modelType == null)
                    return new ErrorResponse<TestInfo>() { ErrorCode = ErrorCode.InvalidId };

                var model = modelType.Model.OrderByDescending(t => t.Id).First();

                if (model == null)
                    return new ErrorResponse<TestInfo>() { ErrorCode = ErrorCode.NoTestsAvailable };

                var test = model.Test.OrderByDescending(t => t.Id).First();

                return new Response<TestInfo>()
                {
                    Data = new TestInfo()
                    {
                        Id = test.Id,
                        ModelId = model.Id,
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
                            Data = new DataSetController().GetDataByDataSetId(t.DataSetId).Data
                        }).ToArray(),
                        TrainingSet = test.TestDataSet.Where(t => t.IsTraningSet == 1).Select(t => new Messages.DataLabelSet()
                        {
                            Id = t.DataSetId,
                            Data = new DataSetController().GetDataByDataSetId(t.DataSetId).Data,
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
        public Response DeleteTest(int id)
        {
            return new Response();
        }
    }
}