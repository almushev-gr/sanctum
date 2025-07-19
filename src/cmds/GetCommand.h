#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Извлечь из хранилища"
  Извлекает из хранилища указанный файл (по пути в хранилище)
  Можно просто указать имя файла\директории
  Тогда система найдет подходящие файла\директории
  Если их окажется больше 1, то система предоставить выбор пользователю
*/
//---
class GetCommand : public Command
{
public:
  GetCommand(core::IfSanctumCore & core);
  virtual ~GetCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
};

}