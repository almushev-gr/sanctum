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

private:
  void ApplyOptionsToResult(const std::vector<std::wstring> & params, core::ContentsOperationResult & result);
  core::ContentsOperationResult GetFileDescriptions(const std::map<std::wstring, std::wstring> & opts) const;
  std::vector<std::wstring> GetTableHeaders(const std::vector<std::wstring> & params);
  std::vector<std::wstring> GetTableLine(const std::vector<std::wstring> & params, const core::FileDescription & fileDesc);

};

}