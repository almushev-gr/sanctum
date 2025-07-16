#pragma once
#include <vector>
#include <string>


namespace sanctum
{

//----------------------------------------------------------
/*
  Таблица для вывода информации в консоль
*/
//---
class ConsoleTable
{
private:
  std::vector<std::wstring> m_header; ///< Заголовок таблицы
  std::vector<std::vector<std::wstring>> m_lines; ///< Строки таблицы

public:
  ConsoleTable(const std::vector<std::wstring> & headerLines);
  ~ConsoleTable() = default;

  void AddLine(const std::vector<std::wstring> & cells);
  std::vector<std::wstring> GetPrintStrings();
  bool IsEmpty() const;

};

}