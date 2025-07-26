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
  

  

  return true;  
}


//----------------------------------------------------------
/*
  Вывести информацию о защите хранилища
*/
//--- 
void KeyCommand::KeyInformation()
{
  core::ProtectionMethod method = GetCore().GetProtectionMethod();

  switch (method)
  {
    case (core::ProtectionMethod::CoreKey):
    {
      AddSuccessMessageStrings({L"Protection method: core key"}); 
    }
    break;

    case (core::ProtectionMethod::EncrypterKey):
    {
      AddSuccessMessageStrings({L"Protection method: encrypter key"}); 
    }
    break;

    case (core::ProtectionMethod::NoKey):
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