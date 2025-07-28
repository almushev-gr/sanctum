#include "KeyCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
KeyCommand::KeyCommand(core::IfSanctumCore & core)
  : Command(core)
{
}


//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool KeyCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();

  if (params.empty()) // информация о ключе
  {
    KeyInformation();
    return true;
  }

  std::map<std::wstring, std::wstring> opts = GetOptions(params);
  
  if (opts.count(L"core"))
  {
    return CoreKeyOperation(opts);
  }

  

  return true;  
}


//----------------------------------------------------------
/*
  Вывести информацию о защите хранилища
*/
//--- 
bool KeyCommand::CoreKeyOperation(const std::map<std::wstring, std::wstring> & opts)
{
  if (opts.count(L"edit"))
  {
    std::optional<std::string> currentCoreKey = EnterKey("Enter current core key: ");

    if (!currentCoreKey)
    {
      return true;
    }

    std::optional<std::string> newCoreKey = EnterKey("Enter new core key: ");

    if (!newCoreKey)
    {
      return true;
    }

    std::optional<std::string> confirmCoreKey = EnterKey("Confirm new core key: ");

    if (!confirmCoreKey)
    {
      return true;
    }

    if (*newCoreKey == *confirmCoreKey)
    {
      core::OperationResult result = GetCore().ChangeCoreKey(*currentCoreKey, *newCoreKey);

      if (result == core::OperationResult::Ok)
      {
        AddSuccessMessageStrings({L"Core key changed successfully"});
        return true;
      }
      else          
      {
        AddFailMessageStrings({L"Invalid current (or new) key entered"});
        return false;
      }
    }
    else
    {
      AddFailMessageStrings({L"Key confirmation failed"});
      return false;
    }
  }

  return false;
}


//----------------------------------------------------------
/*
  Вывести информацию о защите хранилища
*/
//--- 
void KeyCommand::KeyInformation()
{
  core::KeyPolicy method = GetCore().GetKeyPolicy();

  switch (method)
  {
    case (core::KeyPolicy::CoreKey):
    {
      AddSuccessMessageStrings({L"Protection method: core key"}); 
    }
    break;

    case (core::KeyPolicy::EncrypterKey):
    {
      AddSuccessMessageStrings({L"Protection method: encrypter key"}); 
    }
    break;

    case (core::KeyPolicy::NoKey):
    {
      AddSuccessMessageStrings({L"Protection method: no key"}); 
    }
    break;
  }

  if (GetCore().IsPermanentKeyDefined())
  {
    AddSuccessMessageStrings({L"Permanent key is defined"});
  }
  else
  {
    AddSuccessMessageStrings({L"Permanent key is not defined"});
  }
}

//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring KeyCommand::GetName() const
{
  return L"key";
}

}