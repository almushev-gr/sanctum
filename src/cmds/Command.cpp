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

    case core::OperationResult::WrongEncrypter:
      m_failMessage.emplace_back(L"Sanctum encrypted by another encrypter");
    break;

     case core::OperationResult::WrongKeyOrEncrypter:
      m_failMessage.emplace_back(L"Sanctum encrypted by another encrypter or invalid key");
    break;
   
    case core::OperationResult::RemoveFileError:
      m_failMessage.emplace_back(L"Filesystem: cant remove files");
    break;

    case core::OperationResult::RenameFileError:
      m_failMessage.emplace_back(L"Filesystem: cant rename files");
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
EnterKeyResult Command::EnterOperationKey()
{
  std::optional<std::string> key = EnterKey("Key required: ");

  if (!key)
  {
    return EnterKeyResult::Interrupted;
  }
   
  m_core->SetOperationKey(*key);
  return EnterKeyResult::Ok;
}


//----------------------------------------------------------
/*
  Ввести оперативный ключ шифрации с подтверждением
*/
//---
EnterKeyResult Command::EnterConfirmatedOperationKey()
{
  std::optional<std::string> key = EnterKey("Key required: ");

  if (!key)
  {
    return EnterKeyResult::Interrupted;
  }
   
  std::optional<std::string> conirmationKey = EnterKey("Key confirmation: ");

  if (!conirmationKey)
  {
    return EnterKeyResult::Interrupted;
  }
  else if (*key != *conirmationKey)
  {
    AddFailMessageStrings({L"Key confirmation mismatch"});
    return EnterKeyResult::ConfirmationMismatch;
  }

  m_core->SetOperationKey(*key);
  return EnterKeyResult::Ok;
}


//----------------------------------------------------------
/*
  Ввести ключ из консоли
  \return nullopt если был нажат Escape
*/
//---
std::optional<std::string> Command::EnterKey(const std::string & promt)
{
  std::string key;
  std::cout << promt;
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
    else if (ch == 27)
    {
      std::cout << std::endl;
      return std::nullopt;
    }
    else 
    {
      key += ch;
      std::cout << '*'; 
    }
  }

  std::cout << std::endl;
  return key;
}


//----------------------------------------------------------
/*
  Запросить подтверждение
  \param info информация для подтверждения
  \param promt строка с запросом
*/
//---
bool Command::Confirm(const std::vector<std::wstring> & info, const std::wstring & promt) const
{
  for (auto && nextStr : info)
  {
    std::wcout << nextStr << std::endl;
  }

  std::wcout << promt << L" (y\\n):";
  wchar_t answer = _getwch();
  std::wcout << std::endl;

  return answer == L'y' || answer == L'Y' || answer == L'н' || answer == L'Н';
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


//----------------------------------------------------------
/*
  Выбрать ключи (с параметрами, если есть)
*/
//---
std::map<std::wstring,std::wstring> Command::GetOptions(const std::vector<std::wstring> & params)
{
  std::map<std::wstring,std::wstring> result;
  std::wstring currentOption;

  for (auto && nextParam : params)
  {
    if (!nextParam.empty() && nextParam[0] == L'-')
    {
      if (currentOption.empty())
      {
        currentOption = nextParam.substr(1, nextParam.size() - 1);
      }
      else
      {
        result[currentOption] = L"";
        currentOption = nextParam.substr(1, nextParam.size() - 1);
      }
    }
    else if (!currentOption.empty()) // параметр текущей опции
    {
      result[currentOption] = nextParam;
      currentOption.clear();
    }
  }

  if (!currentOption.empty())
  {
    result[currentOption] = L"";
  }

  return result;
}


//----------------------------------------------------------
/*
  Представляет ли результат ошибку работы с ключом шифрации
*/
//---
bool Command::IsKeyError(core::OperationResult result)
{
  return result == core::OperationResult::InvalidKey || result == core::OperationResult::WrongEncrypter 
  || result == core::OperationResult::WrongKeyOrEncrypter || result == core::OperationResult::KeyRequired;
}

}