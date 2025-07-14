#include "Command.h"
#include "IfSanctumCore.h"
#include <iostream>
#include <conio.h>
#include <algorithm>


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
Command::Command(core::IfSanctumCore & core)
  : m_core(&core)
{
}


//----------------------------------------------------------
/*
  Сформировать сообщения для негативного результата
*/
//---
void Command::MakeMessagesForNegativeResult(core::OperationResult result)
{
  switch (result)
  {
    case core::OperationResult::NoSanctum:
      m_failMessage.emplace_back(L"Sanctum access fail");
    break;

    case core::OperationResult::NoWorkFile:
      m_failMessage.emplace_back(L"Workfile is empty");
    break;

    case core::OperationResult::InvalidKey:
      m_failMessage.emplace_back(L"Invalid key");
    break;

    case core::OperationResult::KeyHashDismatch:
      m_failMessage.emplace_back(L"Key hash mismatch. Clear hash or enter suitable key");
    break;

    default:
      m_failMessage.emplace_back(L"Command failed. Unknown error");
  }
}


//----------------------------------------------------------
/*
  Ввести оперативный ключ шифрации
*/
//---
bool Command::EnterOperationKey()
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
  Разрешить конфликт входных данных
  Пользователю предолагается выбрать вручную вариант
*/
//---
std::optional<std::wstring> Command::ResolveAmbiguousInput(const std::vector<std::wstring> & inputs) const
{
  std::wcout << "Command params is ambiguous. Choose manually " << std::endl;

  for (size_t i=0; i<inputs.size(); i++)
  {
    std::wcout << i+1 << ". ";
    std::wcout << inputs[i] << std::endl;
  }

  std::wcout << "Choose variant number (0 for no choose): ";
  int chooseIndex;
  std::wcin >> chooseIndex;
 
  if (chooseIndex > 0 && chooseIndex <= inputs.size())
  {
    return inputs[chooseIndex - 1];
  }
  else if (chooseIndex != 0)
  {
    std::wcout << "Wrong number. Command escaped" << std::endl;
  }

  return std::nullopt;
}


//----------------------------------------------------------
/*
  Очистить все сообщения
*/
//---
void Command::ClearMessages()
{
  ClearSuccessMessage();
  ClearFailMessage();
}


//----------------------------------------------------------
/*
  Добавить строки в успешное сообщение
*/
//---
void Command::AddSuccessMessageStrings(const std::vector<std::wstring> & strs)
{
  std::copy(strs.begin(), strs.end(), std::back_inserter(m_successMessage));
}


//----------------------------------------------------------
/*
  Добавить строки в провальное сообщение
*/
//---
void Command::AddFailMessageStrings(const std::vector<std::wstring> & strs)
{
  std::copy(strs.begin(), strs.end(), std::back_inserter(m_failMessage));
}

}