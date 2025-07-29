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
    return EditCoreKey();
  }
  else if (opts.count(L"drop"))
  {
    return DropCoreKey();
  }
  else if (opts.count(L"check"))
  {
    return CheckCoreKey();
  }

  return false;
}


//----------------------------------------------------------
/*
  Изменить ключ уровня ядра
*/
//--- 
bool KeyCommand::EditCoreKey()
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
    }
  }
  else
  {
    AddFailMessageStrings({L"Key confirmation failed"});
  }

  return false;
}


//----------------------------------------------------------
/*
  Скинуть ключ ядра на умолчательный
*/
//--- 
bool KeyCommand::DropCoreKey()
{
  std::optional<std::string> currentCoreKey = EnterKey("Enter current core key: ");

  if (!currentCoreKey)
  {
    return true;
  }

  if (GetCore().IsCoreKeyValid(*currentCoreKey))
  {
    GetCore().DropCoreKey();
    AddSuccessMessageStrings({L"Core key droped"});
    return true;
  }

  AddFailMessageStrings({L"Invalid core key entered"});
  return false;
}


//----------------------------------------------------------
/*
  Проверить ключ ядра
*/
//--- 
bool KeyCommand::CheckCoreKey()
{
  std::optional<std::string> currentCoreKey = EnterKey("Enter current core key: ");

  if (!currentCoreKey)
  {
    return true;
  }

  if (GetCore().IsCoreKeyValid(*currentCoreKey))
  {
    AddSuccessMessageStrings({L"Core key is valid"});
    return true;
  }

  AddFailMessageStrings({L"Invalid core key"});
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