#pragma once
#include "IfCommand.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Директория хранилища"
  Задает местоположение директории файла-хранилища
*/
//---
class SanctumDirCommand : public IfCommand
{
private:
  core::IfSanctumCore * m_core; ///< ядро хранилища
  std::vector<std::wstring> m_successMessage; ///< сообщение успешного завершения
  std::vector<std::wstring> m_failMessage; ///< сообщение провального завершения
 
public:
  SanctumDirCommand(core::IfSanctumCore & core);
  virtual ~SanctumDirCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
  virtual const std::vector<std::wstring> & GetSuccessMessage() const override { return m_successMessage; }
  virtual const std::vector<std::wstring> & GetFailMessage() const override { return m_failMessage; }
};

}