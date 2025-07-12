#include "PutCommand.h"
#include "IfSanctumCore.h"

#include <algorithm>
#include <iterator>
#include <iostream>


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
  std::wstring fileName;

  if (params.empty())
  {
    fileName = L""; // помещается рабочий файл (workFile)
  }
  else
  {
    fileName = params[0];
  }

  core::FileOperationResult result = m_core->Put(fileName);

  if (result.opResult == core::OperationResult::AmbiguousInput)
  {
    std::optional<std::wstring> concreteFileName = ResolveAmbiguousInput(result.ambiguousFiles);

    if (concreteFileName && params.size() > 0)
    {
      std::vector<std::wstring> runParams = params;
      runParams[0] = *concreteFileName;
      return Run(runParams);
    }
    else 
    {
      return true;
    }
  }
  else if (result.opResult == core::OperationResult::Ok)
  {
    m_successMessage.emplace_back(L"Files put successfully");
    return true;
  }
  else if (result.opResult == core::OperationResult::NoSuchFileOrDir)
  {
    m_failMessage.emplace_back(L"Files not found:");
    std::copy(result.problemFiles.begin(), result.problemFiles.end(), std::back_inserter(m_failMessage));
    return false;
  }
  else if (result.opResult == core::OperationResult::FileProcessFail)
  {
    m_failMessage.emplace_back(L"Files process fail:");
    std::copy(result.problemFiles.begin(), result.problemFiles.end(), std::back_inserter(m_failMessage));
    return false;
  }
  else if (result.opResult == core::OperationResult::NoSanctum)
  {
    m_failMessage.emplace_back(L"Sanctum access fail");
    return false;
  }
  else if (result.opResult == core::OperationResult::NoWorkFile)
  {
    m_failMessage.emplace_back(L"Workfile is empty");
    return false;
  }
    
  return false;
}


//----------------------------------------------------------
/*
  Разрешить конфликт входного файла
  Пользователю предолагается выбрать вручную вариант
*/
//---
std::optional<std::wstring> PutCommand::ResolveAmbiguousInput(const std::vector<std::wstring> & inputFiles) const
{
  std::wcout << "file name is ambiguous. Choose manually " << std::endl;

  for (size_t i=0; i<inputFiles.size(); i++)
  {
    std::wcout << i+1 << ". ";
    std::wcout << inputFiles[i] << std::endl;
  }

  std::wcout << "Choose file number (0 for no choose): ";
  int chooseIndex;
  std::wcin >> chooseIndex;
 
  if (chooseIndex > 0 && chooseIndex <= inputFiles.size())
  {
    return inputFiles[chooseIndex - 1];
  }
  else if (chooseIndex != 0)
  {
    std::wcout << "Wrong file number. Command escaped" << std::endl;
  }

  return std::nullopt;
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