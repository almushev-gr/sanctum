#pragma once
#include "IfCommand.h"
#include <IfSanctumCore.h>
#include <optional>
#include <map>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда общего назначения
  Базовый класс для большинства других команд
*/
//---
class Command : public IfCommand
{
private:
  core::IfSanctumCore * m_core; ///< ядро хранилища
  std::vector<std::wstring> m_successMessage; ///< сообщение успешного завершения
  std::vector<std::wstring> m_failMessage; ///< сообщение провального завершения
 
public:
  Command(core::IfSanctumCore & core);
  virtual ~Command() = default;
  virtual const std::vector<std::wstring> & GetSuccessMessage() const override { return m_successMessage; }
  virtual const std::vector<std::wstring> & GetFailMessage() const override { return m_failMessage; }

protected:
  core::IfSanctumCore & GetCore() const { return *m_core; }
  void AddSuccessMessageStrings(const std::vector<std::wstring> & strs);
  void AddFailMessageStrings(const std::vector<std::wstring> & strs);
  void ClearSuccessMessage() { m_successMessage.clear(); }
  void ClearFailMessage() { m_failMessage.clear(); }
  void ClearMessages();
  bool EnterOperationKey();
  std::optional<std::wstring> ResolveAmbiguousInput(const std::vector<std::wstring> & inputs) const;
  void MakeMessagesForNegativeResult(core::OperationResult result);
  std::map<std::wstring, std::wstring> GetOptions(const std::vector<std::wstring> & params) const;

};

}