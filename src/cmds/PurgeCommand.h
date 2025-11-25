#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Прочистить хранилище"
  При указании атрибутов файла (имени, версии)
  ищет файл в хранилище и ставит ему метку "Под очистку".
  Метка очистки означает, что файл будет физически удален
  из хранилища при запуске purge -finish
*/
//---
class PurgeCommand : public Command
{
public:
  PurgeCommand(core::IfSanctumCore & core);
  virtual ~PurgeCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;

private:
  core::PurgeTarget GetPurgeTarget(const std::vector<std::wstring> & params) const;
  bool MarkFilesAsPurged(const std::vector<std::wstring> & params);
  bool MarkFilesAsActive(const std::vector<std::wstring> & params);
  bool Purge();
};

}