#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Поместить в хранилище"
  Помещает в хранилище указанный файл (по пути)
  Путь к файлу может быть как абсолютный, так и 
  быть задан относительно рабочей директории
*/
//---
class PutCommand : public Command
{
public:
  PutCommand(core::IfSanctumCore & core);
  virtual ~PutCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
};

}