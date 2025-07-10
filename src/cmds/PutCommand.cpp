#include "PutCommand.h"
#include "IfSanctumCore.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
PutCommand::PutCommand(core::IfSanctumCore & core)
  : m_core(&core)
{
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool PutCommand::Run(const std::vector<std::wstring> & params)
{
  m_successMessage.clear();
  m_failMessage.clear();

  if (params.empty())
  {
    m_failMessage.emplace_back(L"Empty file path");
    return false;
  }

  core::FileOperationResult result = m_core->Put(params[0]);

  if (result.opResult == core::OperationResult::Ok)
  {
    m_successMessage.emplace_back(L"Files put successfully");
    return true;
  }
  else if (result.opResult == core::OperationResult::NoSuchFileOrDir)
  {
    m_failMessage.emplace_back(L"File not found");
  }
  else if (result.opResult == core::OperationResult::FileProcessFail)
  {
    m_failMessage.emplace_back(L"File process fail");
  }

  return false;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring PutCommand::GetName() const
{
  return L"put";
}

}