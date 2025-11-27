#pragma once
#include <string>
#include <memory>
#include <vector>


namespace sanctum
{

//----------------------------------------------------------
/*
  Интерфейс команды для консоли
*/
//---
struct IfCommand
{
  virtual ~IfCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) = 0;
  virtual const std::wstring GetName() const = 0;
  virtual const std::vector<std::wstring> & GetSuccessMessage() const = 0;
  virtual const std::vector<std::wstring> & GetFailMessage() const = 0;
  virtual std::vector<std::wstring> GetSummaryInfo() const = 0;
  virtual std::vector<std::wstring> GetDetailInfo() const = 0;
};

using CommandPtr = std::unique_ptr<IfCommand>;

}