//------------------------------------------------------------------------------
// <auto-generated>
//     This code was generated from a template.
//
//     Manual changes to this file may cause unexpected behavior in your application.
//     Manual changes to this file will be overwritten if the code is regenerated.
// </auto-generated>
//------------------------------------------------------------------------------

namespace DataHub.Models
{
    using System;
    using System.Collections.Generic;
    
    public partial class TestDataSet
    {
        public int Id { get; set; }
        public int TestId { get; set; }
        public int DataSetId { get; set; }
        public int IsTraningSet { get; set; }
        public int IsTestSet { get; set; }
    
        public virtual DataSet DataSet { get; set; }
        public virtual Test Test { get; set; }
    }
}
