#include "cmds/CommandNomenclature.h"
#include <iostream>
#include <IfSanctumCore.h>
#include <cmds/IfCommand.h>
#include <cmds/Command.h>
#include <regex>
#include <windows.h>
#include <locale>


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


//----------------------------------------------------------
/*
  Обработчик прогресса операций
*/
//---
void ProgressHandler(int totalItems, int currentItem)
{
  constexpr int barWidth = 60;
  double percentage = static_cast<double>(currentItem) / static_cast<double>(totalItems);
  int pos = barWidth * percentage;

  std::wcout << L"\r[";
  
  for (int i=0; i<pos; i++)
  {
    std::wcout << L"=";
  }

  for (int i=pos; i<barWidth; i++)
  {
    std::wcout << L" ";
  }

  std::wcout << L"] " << currentItem << L"/" << totalItems;

  if (currentItem == totalItems)
  {
    std::wcout << std::endl;
  }
}


//----------------------------------------------------------
/*
  Получить справку для команды
*/
//---
std::vector<std::wstring> GetCmdHelp(const sanctum::IfCommand & cmd)
{
  std::vector<std::wstring> helpLines = cmd.GetSummaryInfo();
  const std::vector<std::wstring> detailInfo = cmd.GetDetailInfo();
        
  if (!detailInfo.empty())
  {
    helpLines.push_back(L"");
    helpLines.push_back(L"Подробное описание:");
    std::copy(detailInfo.begin(), detailInfo.end(), std::back_inserter(helpLines));
  }

  return helpLines;
}


int main()
{
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);
  setlocale(LC_ALL, "Russian");
  std::unique_ptr<sanctum::core::IfSanctumCore> core = sanctum::core::CreateSanctumCore();
  
  if (!core)
  {
    PrintMessage({L"Cant create core"}, MessageType::Fail);
    std::wcin.get();
    return 1;
  }
  
  core->SetProgressHandler(ProgressHandler);
  sanctum::CommandNomenclature commands(*core);

  while (true)
  {
    std::wstring command;
    std::wcout << L"sanctum: ";
    std::getline(std::ws(std::wcin), command);

    if (command == L"exit")
    {
      break;
    }

    if (command == L"cls")
    {
      std::wcout.clear();
      system("cls");
      continue;
    }

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
      std::map<std::wstring,std::wstring> opts = sanctum::Command::GetOptions(tokens);

      if (opts.count(L"help"))
      {
        const std::vector<std::wstring> cmdHelpInfo = GetCmdHelp(*cmd);
        
        if (!cmdHelpInfo.empty())
        {
          PrintMessage(cmdHelpInfo, MessageType::Success);
        }
      }
      else if (cmd->Run(tokens))
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
  }
  
  return 0;
}

  
