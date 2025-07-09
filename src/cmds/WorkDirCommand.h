#pragma once
#include "IfCommand.h"
#include <IfSanctumCore.h>


namespace sanctum
{

//----------------------------------------------------------
/*
  Команда "Рабочая директория"
  Задает местоположение рабочей директории приложения
  Файлы, извлекаемые из хранилища попадают в рабочую директорию
  При помещении файлов в хранилище по относительному пути,
  происходит поиск этих файлов в рабочей директории
*/
//---
class WorkDirCommand : public IfCommand
{
private:
  core::IfSanctumCore * m_core; ///< ядро хранилища
  std::vector<std::wstring> m_successMessage; ///< сообщение успешного завершения
  std::vector<std::wstring> m_failMessage; ///< сообщение провального завершения
 
public:
  WorkDirCommand(core::IfSanctumCore & core);
  virtual ~WorkDirCommand() = default;
  virtual bool Run(const std::vector<std::wstring> & params) override;
  virtual const std::wstring GetName() const override;
  virtual const std::vector<std::wstring> & GetSuccessMessage() const override { return m_successMessage; }
  virtual const std::vector<std::wstring> & GetFailMessage() const override { return m_failMessage; }
};

}