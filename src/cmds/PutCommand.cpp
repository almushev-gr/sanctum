#include "PutCommand.h"
#include "IfSanctumCore.h"

#include <algorithm>
#include <iterator>
#include <iostream>
#include <conio.h>


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

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    if (EnterOperationKey())
    {
      result = m_core->Put(fileName);
    }
    else 
    {
      return true;
    }
  }

  if (result.opResult == core::OperationResult::Ok)
  {
    m_successMessage.emplace_back(L"Files put successfully");
    return true;
  }
  else if (result.opResult == core::OperationResult::AmbiguousInput)
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
  else if (result.opResult == core::OperationResult::NoSuchFileOrDir)
  {
    m_failMessage.emplace_back(L"Files not found:");
    std::copy(result.problemFiles.begin(), result.problemFiles.end(), std::back_inserter(m_failMessage));
  }
  else if (result.opResult == core::OperationResult::FileProcessFail)
  {
    m_failMessage.emplace_back(L"Files process fail:");
    std::copy(result.problemFiles.begin(), result.problemFiles.end(), std::back_inserter(m_failMessage));
  }
  else if (result.opResult == core::OperationResult::NoSanctum)
  {
    m_failMessage.emplace_back(L"Sanctum access fail");
  }
  else if (result.opResult == core::OperationResult::NoWorkFile)
  {
    m_failMessage.emplace_back(L"Workfile is empty");
  }
  else if (result.opResult == core::OperationResult::InvalidKey)
  {
    m_failMessage.emplace_back(L"Invalid key");
  }
  else if (result.opResult == core::OperationResult::KeyHashDismatch)
  {
    m_failMessage.emplace_back(L"Key hash mismatch. Clear hash or enter suitable key");
  }
    
  return false;
}


//----------------------------------------------------------
/*
  Ввести оперативный ключ шифрации
*/
//---
bool PutCommand::EnterOperationKey()
{
  std::string key;
  std::cout << "Key required: ";
  char ch;
  
  while ((ch = _getch()) != '\r') // while not Enter
  { 
    if (ch == '\b') // backspace
    { 
      if (!key.empty()) 
      {
        std::cout << "\b \b"; // Erase character and move cursor back
        key.pop_back();
      }
    } 
    else 
    {
      key += ch;
      std::cout << '*'; 
    }
  }
  
  std::cout << std::endl;
  m_core->SetOperationKey(key);
  return true;
}


//----------------------------------------------------------
/*
  Разрешить конфликт входного файла
  Пользователю предолагается выбрать вручную вариант
*/
//---
std::optional<std::wstring> PutCommand::ResolveAmbiguousInput(const std::vector<std::wstring> & inputFiles) const
{
  std::wcout << "File name is ambiguous. Choose manually " << std::endl;

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