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
    public class ModelController : ApiController
    {
        [HttpGet]
        [Route("api/model")]
        public Response<Messages.Model[]> GetAllModels()
        {
            return new Response<Messages.Model[]>() { Data = new Entities().Model.Select(m => new Messages.Model() { Id = m.Id, Name = m.Name, TypeName = m.ModelType.Name }).ToArray() };
        }

        [HttpGet]
        [Route("api/model/{id}")]
        public Response<Messages.Model> GetModelById(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.Model>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var model = db.Model.FirstOrDefault(m => m.Id == id);

                if (model == null)
                    return new Response<Messages.Model>() { ErrorCode = ErrorCode.InvalidId };

                return new Response<Messages.Model>()
                {
                    Data = new Messages.Model() { Id = model.Id, Name = model.Name, TypeName = model.ModelType.Name }
                };
            }
        }

        [HttpGet]
        [Route("api/modeltype")]
        public Response<Messages.ModelType[]> GetAllModelTypes()
        {
            return new Response<Messages.ModelType[]>() { Data = new Entities().ModelType.Select(m => new Messages.ModelType() { Id = m.Id, Name = m.Name }).ToArray() };
        }

        [HttpGet]
        [Route("api/modeltype/{id}")]
        public Response<Messages.ModelType> GetModelTypeById(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.ModelType>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var model = db.ModelType.FirstOrDefault(m => m.Id == id);

                if (model == null)
                    return new Response<Messages.ModelType>() { ErrorCode = ErrorCode.InvalidId };

                return new Response<Messages.ModelType>()
                {
                    Data = new Messages.ModelType() { Id = model.Id, Name = model.Name }
                };
            }
        }

        [HttpGet]
        [Route("api/model/{id}/parameter")]
        public Response<Messages.Parameter[]> GetModelParameters(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.Parameter[]>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var model = db.Model.FirstOrDefault(m => m.Id == id);

                if (model == null)
                    return new Response<Messages.Parameter[]>() { ErrorCode = ErrorCode.InvalidId };

                return new Response<Messages.Parameter[]>()
                {
                    Data = model.Parameter.Select(p => new Messages.Parameter()
                    {
                        Id = p.Id,
                        Name = p.Property.Name,
                        PropertyId = p.PropertyId,
                        Value = p.Value
                    }).ToArray()
                };
            }
        }

        [HttpGet]
        [Route("api/modeltype/{id}/property")]
        public Response<Messages.Property[]> GetModelTypeProperties(int? id)
        {
            if (id == null)
                return new ErrorResponse<Messages.Property[]>() { ErrorCode = ErrorCode.InvalidId };

            using (Entities db = new Entities())
            {
                var model = db.ModelType.FirstOrDefault(m => m.Id == id);

                if (model == null)
                    return new Response<Messages.Property[]>() { ErrorCode = ErrorCode.InvalidId };

                return new Response<Messages.Property[]>()
                {
                    Data = model.Property.Select(p => new Messages.Property()
                    {
                        Id = p.Id,
                        Name = p.Name,
                        Type = p.Type
                    }).ToArray()
                };
            }
        }

        [HttpPost]
        [Route("api/model")]
        public Response<Messages.Model> AddModel(Messages.NewModel newModel)
        {
            using (Entities db = new Entities())
            {
                var added = db.Model.Add(new Models.Model()
                {
                    ModelTypeId = newModel.TypeId,
                    Name = newModel.Name,
                    Parameter = newModel.Parameters.Select(p => new Models.Parameter()
                    {
                        PropertyId = p.PropertyId,
                        Value = p.Value
                    }).ToArray()
                });

                db.SaveChanges();

                return GetModelById(added.Id);
            }
        }

        [HttpPost]
        [Route("api/modeltype")]
        public Response<Messages.ModelType> AddModelType(Messages.NewModelType newModel)
        {
            using (Entities db = new Entities())
            {
                var added = db.ModelType.Add(new Models.ModelType()
                {
                    CreatedDate = DateTime.Now,
                    Name = newModel.Name,
                    Property = newModel.Properties.Select(p => new Models.Property()
                    {
                        Name = p.Name,
                        Type = p.Type
                    }).ToArray()
                });
                db.SaveChanges();

                return GetModelTypeById(added.Id);
            }
        }

        [HttpDelete]
        [Route("api/model/{id}")]
        public Response DeleteModel()
        {
            return new ErrorResponse();
        }

        [HttpDelete]
        [Route("api/modeltype/{id}")]
        public Response DeleteModelType()
        {
            return new ErrorResponse();
        }
    }
}
