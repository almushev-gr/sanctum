#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда управления шифратором. Может:
  1) загружать внешний шифратор
  2) выгружать внешний шифратор с возвратом на умолчательный
  3) показывать текущий шифратор
 */
//---
class EncryptorCommand : public Command
{
public:
  EncryptorCommand(core::IfSanctumCore & core);
  virtual ~EncryptorCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
};

}