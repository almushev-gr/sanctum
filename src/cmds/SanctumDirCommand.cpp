#include "SanctumDirCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
SanctumDirCommand::SanctumDirCommand(core::IfSanctumCore & core)
  : m_core(&core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool SanctumDirCommand::Run(const std::vector<std::wstring> & params)
{
  m_successMessage.clear();
  m_failMessage.clear();

  if (params.empty())
  {
    m_successMessage.emplace_back(L"Current Sanctum directory: " + m_core->GetSanctumDir());
    return true;
  }

  core::OperationResult result = m_core->SetSanctumDir(params[0]);

  if (result == core::OperationResult::Ok)
  {
    m_successMessage.emplace_back(L"Current Sanctum directory: " + m_core->GetSanctumDir());
    return true;
  }
  else if (result == core::OperationResult::NoSuchFileOrDir)
  {
    m_failMessage.emplace_back(L"Directory not found");
  }

  return false;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring SanctumDirCommand::GetName() const
{
  return L"sdir";
}

}