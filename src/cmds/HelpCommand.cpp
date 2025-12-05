#include "HelpCommand.h"


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
HelpCommand::HelpCommand(const std::vector<CommandPtr> & cmds)
  : Command()
{
  for (auto && nextCmd : cmds)
  {
    std::vector<std::wstring> summaryInfo = nextCmd->GetSummaryInfo();

    for (auto && nextSummaryLine : summaryInfo)
    {
      nextSummaryLine = L"\t" + nextSummaryLine;
    }

    m_cmdInfo[nextCmd->GetName()] = summaryInfo;
  }
}

//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool HelpCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
  std::map<std::wstring, std::wstring> opts = GetOptions(params);
  AddSuccessMessageStrings({L"Доступные команды:"});
  int cmdNumber = 0;

  for (auto & [cmdName, cmdInfo] : m_cmdInfo)
  {
    cmdNumber++;
    AddSuccessMessageStrings({std::to_wstring(cmdNumber) + L") " + cmdName});

    if (!opts.count(L"list"))
    {
      AddSuccessMessageStrings(cmdInfo);
    }
  }
  
  return true;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring HelpCommand::GetName() const
{
  return L"help";
}

//----------------------------------------------------------
/*
  Получить краткую информацию о команде
*/
//--- 
std::vector<std::wstring> HelpCommand::GetSummaryInfo() const
{
  std::vector<std::wstring> result;
  result.push_back(L"Формат: help [-list]");
  result.push_back(L"Служит для вывода списка команд и краткой информации о них");
  return result;
}


//----------------------------------------------------------
/*
  Детальную информацию о команде
*/
//--- 
std::vector<std::wstring> HelpCommand::GetDetailInfo() const
{
  std::vector<std::wstring> result;
  result.push_back(L"[-list] вывести только список команд (без краткой справки о каждой)");
  return result;
}

}