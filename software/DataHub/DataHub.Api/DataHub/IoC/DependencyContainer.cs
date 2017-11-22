using Microsoft.Practices.Unity;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DataHub.IoC
{
    public class ServiceContainer : UnityContainer
    {
        public void AddService<TInterface, TClass>() where TClass : TInterface
        {
            this.RegisterType<TInterface, TClass>(new HierarchicalLifetimeManager());
        }
    }
}
