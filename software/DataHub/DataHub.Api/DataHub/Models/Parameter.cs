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
    
    public partial class Parameter
    {
        public int Id { get; set; }
        public string Value { get; set; }
        public int ModelId { get; set; }
        public int PropertyId { get; set; }
    
        public virtual Model Model { get; set; }
        public virtual Property Property { get; set; }
    }
}
