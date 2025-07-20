#include "EncryptorCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
EncryptorCommand::EncryptorCommand(core::IfSanctumCore & core)
  : Command(core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool EncryptorCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();

  if (params.empty())
  {
    AddSuccessMessageStrings({L"Current encrypter: " + GetCore().GetEncrypterName()});
    return true;
  }

  std::map<std::wstring,std::wstring> opts = GetOptions(params);

  if (opts.count(L"un"))
  {
    core::OperationResult unloadResult = GetCore().UnloadEncrypter();

    if (unloadResult == core::OperationResult::Ok)
    {
      AddSuccessMessageStrings({L"Encrypter unloaded successfully"});
      return true;
    }
    else if (unloadResult == core::OperationResult::OutsideEncrypterNotLoaded)
    {
      AddFailMessageStrings({L"Outside encryptor not loaded"});
      return false;
    }
    else 
    {
      AddFailMessageStrings({L"Unload unknown error"});
      return false;
    }
  }
  
  std::wstring encPath = params[0];
  core::OperationResult result = GetCore().LoadEncrypter(encPath);

  if (result == core::OperationResult::Ok)
  {
    AddSuccessMessageStrings({L"Loaded encrypter name: " + GetCore().GetEncrypterName()});
    return true;
  }
  else if (result == core::OperationResult::NoSuchFileOrDir)
  {
    AddFailMessageStrings({L"Encryptor library not found"});
  }
  else if (result == core::OperationResult::OutsideEncrypterAlreadyLoaded)
  {
    AddFailMessageStrings({L"Outside encryptor alreadey loaded"});
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
const std::wstring EncryptorCommand::GetName() const
{
  return L"enc";
}

}