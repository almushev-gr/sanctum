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

  core::OperationResult result = GetCore().SetSanctumName(params[0]);

  if (result == core::OperationResult::Ok)
  {
    AddSuccessMessageStrings({L"Current Sanctum name: " + GetCore().GetSanctumName()});
    return true;
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