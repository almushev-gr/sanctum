#include "WorkDirCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
WorkDirCommand::WorkDirCommand(core::IfSanctumCore & core)
  : m_core(&core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool WorkDirCommand::Run(const std::vector<std::wstring> & params)
{
  m_successMessage.clear();
  m_failMessage.clear();

  if (params.empty())
  {
    m_successMessage.emplace_back(L"Current work directory: " + m_core->GetWorkDir());
    return true;
  }

  core::OperationResult result = m_core->SetWorkDir(params[0]);

  if (result == core::OperationResult::Ok)
  {
    m_successMessage.emplace_back(L"Current work directory: " + m_core->GetWorkDir());
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
const std::wstring WorkDirCommand::GetName() const
{
  return L"wdir";
}


//----------------------------------------------------------
/*
  Получить краткую информацию о команде
*/
//--- 
std::vector<std::wstring> WorkDirCommand::GetSummaryInfo() const
{
  std::vector<std::wstring> result;
  return result;
}


//----------------------------------------------------------
/*
  Детальную информацию о команде
*/
//--- 
std::vector<std::wstring> WorkDirCommand::GetDetailInfo() const
{
  std::vector<std::wstring> result;
  return result;
}

}