#include "PutCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
PutCommand::PutCommand(core::IfSanctumCore & core)
  : Command(core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool PutCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
  std::wstring fileName;

  if (params.empty())
  {
    fileName = L""; // помещается рабочий файл (workFile)
  }
  else
  {
    fileName = params[0];
  }

  core::FileOperationResult result = GetCore().Put(fileName);

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    if (EnterOperationKey())
    {
      result = GetCore().Put(fileName);
    }
    else 
    {
      return true;
    }
  }

  if (result.opResult == core::OperationResult::Ok)
  {
    AddSuccessMessageStrings({L"Files put successfully"});
    return true;
  }
  else if (result.opResult == core::OperationResult::AmbiguousInput)
  {
    std::optional<std::wstring> concreteFileName = ResolveAmbiguousInput(result.ambiguousFiles);

    if (concreteFileName && params.size() > 0)
    {
      std::vector<std::wstring> runParams = params;
      runParams[0] = *concreteFileName;
      return Run(runParams);
    }
    else 
    {
      return true;
    }
  }
  else if (result.opResult == core::OperationResult::NoSuchFileOrDir)
  {
    AddFailMessageStrings({L"Files not found:"});
    AddFailMessageStrings(result.problemFiles);
  }
  else if (result.opResult == core::OperationResult::FileProcessFail)
  {
    AddFailMessageStrings({L"Files process fail:"});
    AddFailMessageStrings(result.problemFiles);
  }
  else
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
  }
    
  return false;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring PutCommand::GetName() const
{
  return L"put";
}

}