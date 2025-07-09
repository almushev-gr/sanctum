#include "cmds/CommandNomenclature.h"
#include <iostream>
#include <IfSanctumCore.h>
#include <cmds/IfCommand.h>
#include <regex>


// Тип воможного сообщения 
enum class MessageType
{
  Info,
  Success,
  Fail
};


//----------------------------------------------------------
/*
  Вывести сообщение
*/
//---
void PrintMessage(const std::vector<std::wstring> & messageLines, MessageType messageType)
{
  for (auto && nextLine : messageLines)
  {
    switch (messageType)
    {
      case MessageType::Info: 
      std::wcout << L"[INFO]: ";
      break;
      case MessageType::Success: 
      std::wcout << L"[OK]: ";
      break;
      case MessageType::Fail: 
      std::wcout << L"[FAIL]: ";
      break;
    }

    std::wcout << nextLine << std::endl;
  }
}



int main()
{
  setlocale(LC_ALL, "Russian");
  std::unique_ptr<sanctum::core::IfSanctumCore> core = sanctum::core::CreateSanctumCore();
  
  if (!core)
  {
    PrintMessage({L"Cant create core"}, MessageType::Fail);
    std::wcin.get();
    return 1;
  }
  
  sanctum::CommandNomenclature commands(*core);

  while (true)
  {
    std::wstring command;
    std::wcout << L"sanctum: ";
    std::getline(std::wcin, command);

    std::wregex regex_space(L"\\s+");
    std::wsregex_token_iterator iter (command.begin(), command.end(), regex_space, -1);
    std::wsregex_token_iterator end;
    std::vector<std::wstring> tokens;

    for (; iter != end; ++iter) 
    {
      tokens.push_back(*iter);
    }

    if (tokens.empty())
    {
      continue;
    }

    if (sanctum::IfCommand * cmd = commands.Get(tokens[0]))
    {
      tokens.erase(tokens.begin());

      if (cmd->Run(tokens))
      {
        PrintMessage(cmd->GetSuccessMessage(), MessageType::Success);
      }
      else
      {
        PrintMessage(cmd->GetFailMessage(), MessageType::Fail);
      }
    }
    else
    {
      PrintMessage({L"Command doesnt exist"}, MessageType::Fail);
    }
  
    if (command == L"exit")
    {
      break;
    }
  }
  
  return 0;
}

  
