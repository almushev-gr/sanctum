#pragma once
#include "IfCommand.h"
#include <IfSanctumCore.h>
#include <vector>
#include <string>


namespace sanctum
{

//----------------------------------------------------------
/*
  Вся доступная номенклатура команд консоли
*/
//---
class CommandNomenclature
{
private:
  std::vector<CommandPtr> m_cmds; ///< перечень команд
  core::IfSanctumCore * m_core; ///< ядро хранилища

public:
  CommandNomenclature(core::IfSanctumCore & core);
  ~CommandNomenclature() = default;

  IfCommand * Get(const std::wstring & cmdName) const;
};

}