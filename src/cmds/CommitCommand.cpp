#include "CommitCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
CommitCommand::CommitCommand(core::IfSanctumCore & core)
  : Command(core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool CommitCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
  core::OperationResult result = GetCore().Commit();

  if (result == core::OperationResult::Ok)
  {
    AddSuccessMessageStrings({L"Commit successfully"});
    return true;
  }
  else if (result == core::OperationResult::NoSanctum)
  {
    AddFailMessageStrings({L"No data to commit"});
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
const std::wstring CommitCommand::GetName() const
{
  return L"commit";
}

}