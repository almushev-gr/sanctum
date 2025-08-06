#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Ключ хранилища"
  Используется для смены\проверки\сброса ключей хранилища
*/
//---
class KeyCommand : public Command
{
public:
  KeyCommand(core::IfSanctumCore & core);
  virtual ~KeyCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;

private:
  void KeyInformation();
  bool CoreKeyOperation(const std::map<std::wstring, std::wstring> & opts);
  bool PermanentKeyOperation(const std::map<std::wstring, std::wstring> & opts);
  bool EditCoreKey();
  bool DropCoreKey();
  bool CheckCoreKey();
  bool EditPermanentKey();
  bool DropPermanentKey();
  bool CheckKey();
};

}