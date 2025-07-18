#pragma once
#include "Command.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Зафиксировать изменения"
  Переносит изменения в "фантомной" копии хранилища
  в основное хранилище
*/
//---
class CommitCommand : public Command
{
public:
  CommitCommand(core::IfSanctumCore & core);
  virtual ~CommitCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
};

}