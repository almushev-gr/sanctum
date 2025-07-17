#include "ConsoleTable.h"
#include <iterator>


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//---
ConsoleTable::ConsoleTable(const std::vector<std::wstring> & headerLines)
  : m_header(headerLines)
  , m_lines()
{
}


//----------------------------------------------------------
/*
  Добавить строку
*/
//---
void ConsoleTable::AddLine(const std::vector<std::wstring> & cells)
{
  m_lines.push_back(cells);
}


//----------------------------------------------------------
/*
  Проверить пуста ли таблица
*/
//---
bool ConsoleTable::IsEmpty() const 
{
  return m_lines.empty();  
}

namespace
{

//----------------------------------------------------------
/*
  Сформировать строку списка для вывода
*/
//--- 
std::wstring MakeString(const std::vector<std::wstring> & cells, const std::vector<int> & widths)
{
  std::wstring result;

  for (size_t iCell=0; iCell<cells.size(); iCell++)
  {
    result += cells[iCell];

    if (cells[iCell].size() < widths[iCell])
    {
      std::wstring spaces(widths[iCell] - cells[iCell].size(), L' ');
      std::copy(spaces.begin(), spaces.end(), std::back_inserter(result));
    }
  }
  
  return result;
} 


//----------------------------------------------------------
/*
  Сформировать разделительную строку
*/
//--- 
std::wstring MakeSeparator(const std::vector<int> & widths)
{
  std::wstring result;

  for (int nextWidth : widths)
  {
    std::wstring sep(nextWidth, L'-');
    std::copy(sep.begin(), sep.end(), std::back_inserter(result));
  }

  return result;
}

}


//----------------------------------------------------------
/*
  Получать строки таблицы для печати
*/
//---
std::vector<std::wstring> ConsoleTable::GetPrintStrings()
{
  constexpr int c_columnGap = 2;
  std::vector<int> columnWidth(m_header.size(), 0);

  for (size_t i=0; i<m_header.size(); i++)
  {
    columnWidth[i] = m_header[i].size() + c_columnGap;

    for (size_t j=0; j<m_lines.size(); j++)
    {
      if (m_lines[j][i].size() + c_columnGap > columnWidth[i])
      {
        columnWidth[i] = m_lines[j][i].size() + c_columnGap;
      }
    }
  }

  std::vector<std::wstring> printStrings;
  std::wstring headerString = MakeString(m_header, columnWidth);
  printStrings.push_back(headerString);
  std::wstring separatorString = MakeSeparator(columnWidth);
  printStrings.push_back(separatorString);

  for (size_t i=0; i<m_lines.size(); i++)
  {
    printStrings.push_back(MakeString(m_lines[i], columnWidth));
  }

  if (m_underSeparator)
  {
    printStrings.push_back(separatorString);
  }

  return printStrings;
}

}