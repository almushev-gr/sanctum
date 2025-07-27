#include "SanctumNameCommand.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
SanctumNameCommand::SanctumNameCommand(core::IfSanctumCore & core)
  : Command(core)
{
}


//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool SanctumNameCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
 
  if (params.empty())
  {
    AddSuccessMessageStrings({L"Current Sanctum name: " + GetCore().GetSanctumName()});
    return true;
  }

  std::map<std::wstring, std::wstring> opts = GetOptions(params);
  core::OperationResult result;

  if (opts.count(L"r"))
  {
    result = GetCore().RenameSanctum(params[0]);

    if (result == core::OperationResult::Ok)
    {
      AddSuccessMessageStrings({L"Sanctum renamed successfully"});
    }
  }
  else
  {
    result = GetCore().SetSanctumName(params[0]);
  }

  if (result == core::OperationResult::Ok)
  {
    AddSuccessMessageStrings({L"Current Sanctum name: " + GetCore().GetSanctumName()});
    return true;
  }
  else if (result == core::OperationResult::FileAlreadyExist)
  {
    AddFailMessageStrings({L"Sanctum with this name already exists"});
  }
  else 
  {
    Command::MakeMessagesForNegativeResult(result);
  }
 
  return false;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring SanctumNameCommand::GetName() const
{
  return L"sname";
}

}