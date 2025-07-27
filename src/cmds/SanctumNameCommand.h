#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Имя хранилища"
  Задает имя файла-хранилища
*/
//---
class SanctumNameCommand : public Command
{
public:
  SanctumNameCommand(core::IfSanctumCore & core);
  virtual ~SanctumNameCommand() = default;
  
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
};

}