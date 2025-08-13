#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Проверить хранилище"
  Последовательно считывает файлы из хранилища
  и проверяет их контрольные суммы
*/
//---
class CheckCommand : public Command
{
public:
  CheckCommand(core::IfSanctumCore & core);
  virtual ~CheckCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
};

}