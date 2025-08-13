#include "CheckCommand.h"
#include "Command.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
CheckCommand::CheckCommand(core::IfSanctumCore & core)
  : Command(core)
{
}


//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool CheckCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
 
  core::FileOperationResult checkResult = GetCore().CheckFiles();

  if (checkResult.opResult == core::OperationResult::KeyRequired)
  {
    EnterKeyResult enterResult = EnterOperationKey();

    if (enterResult == EnterKeyResult::Ok)
    {
      checkResult = GetCore().CheckFiles();
    }
    else 
    {
      return true;
    }
  }

  if (checkResult.opResult == core::OperationResult::Ok)
  {
    AddSuccessMessageStrings({L"All checksums match"});
    return true;
  }
  else if (checkResult.opResult == core::OperationResult::FileProcessFail)
  {
    AddFailMessageStrings({L"Found checksum mismatch"});
    AddFailMessageStrings(checkResult.problemFiles);
  }
  else
  {
    Command::MakeMessagesForNegativeResult(checkResult.opResult);
  }
 
  return false;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring CheckCommand::GetName() const
{
  return L"check";
}

}