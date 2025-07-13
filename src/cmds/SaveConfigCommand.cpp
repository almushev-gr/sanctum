#include "SaveConfigCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
SaveConfigCommand::SaveConfigCommand(core::IfSanctumCore & core)
  : m_core(&core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool SaveConfigCommand::Run(const std::vector<std::wstring> & params)
{
  m_successMessage.clear();
  m_failMessage.clear();

  if (m_core->SaveConfig())
  {
    m_successMessage.emplace_back(L"Config saved successfully");
    return true;
  }

  m_failMessage.emplace_back(L"Config save failed");
  return false;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring SaveConfigCommand::GetName() const
{
  return L"savecfg";
}

}