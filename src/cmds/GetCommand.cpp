#include "GetCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
GetCommand::GetCommand(core::IfSanctumCore & core)
  : Command(core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool GetCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
  std::wstring fileName;

  if (params.empty())
  {
    fileName = L"";
  }
  else
  {
    fileName = params[0];
  }

  core::FileOperationResult result = GetCore().Get(fileName);

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    if (EnterOperationKey())
    {
      result = GetCore().Get(fileName);
    }
    else 
    {
      return true;
    }
  }

  if (result.opResult == core::OperationResult::Ok)
  {
    AddSuccessMessageStrings({L"Files get successfully"});
    return true;
  }
  else if (result.opResult == core::OperationResult::AmbiguousInput)
  {
    std::optional<std::wstring> concreteInput = ResolveAmbiguousInput(result.ambiguousFiles);

    if (concreteInput && params.size() > 0)
    {
      std::vector<std::wstring> runParams = params;
      runParams[0] = *concreteInput;
      return Run(runParams);
    }
    else 
    {
      return true;
    }
  }
  else if (result.opResult == core::OperationResult::FileAlreadyExist)
  {
    AddFailMessageStrings({L"Files already exist:"});
    AddFailMessageStrings(result.problemFiles);
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
const std::wstring GetCommand::GetName() const
{
  return L"get";
}

}