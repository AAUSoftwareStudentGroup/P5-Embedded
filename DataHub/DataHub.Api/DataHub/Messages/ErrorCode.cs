using System;
using System.Collections.Generic;
using System.Linq;
using System.Web;

namespace DataHub.Messages
{
    public enum ErrorCode
    {
        NoError,
        MissingDataFile,
        WrongFileType,
        CouldNotReadFile,
        InvalidId,
        NotFound,
        DataSetNotFound,
        CouldNotReadData,
        InvalidLabelId,
        NoTestsAvailable,
        NameAlreadyInUse,
        DataSetInUse,
        LabelUsedInTests,
        TestNotFound
    }
}