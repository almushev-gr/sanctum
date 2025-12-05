#pragma once
#include "Command.h"

#include <map>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Помощь"
  Выводит перечень доступных команд
  и опционально краткую справку для этих команд
*/
//---
class HelpCommand : public Command
{
private:
  std::map<std::wstring, std::vector<std::wstring>> m_cmdInfo; ///< информация обо всех командах
  
public:
  HelpCommand(const std::vector<CommandPtr> & cmds);
  virtual ~HelpCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
  virtual std::vector<std::wstring> GetSummaryInfo() const override;
  virtual std::vector<std::wstring> GetDetailInfo() const override;
};

}