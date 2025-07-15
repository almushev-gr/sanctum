#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Содержимое хранилища"
  Используется для обзора содержимого хранилища
*/
//---
class ContentCommand : public Command
{
public:
  ContentCommand(core::IfSanctumCore & core);
  virtual ~ContentCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
};

}