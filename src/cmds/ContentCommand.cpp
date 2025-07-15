#include "ContentCommand.h"
#include "IfSanctumCore.h"
#include <iterator>
#include <string>
#include <algorithm>


// не плохо бы сделать флажок -u чтобы показывать только максимальную версию файла в списке
// нужен флажок -ver (узнать текущую версию файла)
// нужен флажок фильтра по имени файла -f

namespace 
{
  constexpr int c_columnGap = 2;
  constexpr wchar_t c_versionColumnHeader[] = L"Ver";
  constexpr wchar_t c_fileNameColumnHeader[] = L"File";
  constexpr wchar_t c_dirColumnHeader[] = L"Dir";
}


namespace sanctum
{

//----------------------------------------------------------
/*
  
*/
//--- 
ContentCommand::ContentCommand(core::IfSanctumCore & core)
  : Command(core)
{
}


namespace 
{

std::wstring MakeString(const std::wstring & version, const std::wstring & fileName, 
    const std::wstring & dirName, int verColumnWidth, int fileNameColumnWidth)
{
  std::wstring result = version;

  if (version.size() < verColumnWidth)
  {
    std::wstring spaces(verColumnWidth - version.size(), L' ');
    std::copy(spaces.begin(), spaces.end(), std::back_inserter(result));
  }

  result += fileName;

  if (fileName.size() < fileNameColumnWidth)
  {
    std::wstring spaces(fileNameColumnWidth - fileName.size(), L' ');
    std::copy(spaces.begin(), spaces.end(), std::back_inserter(result));
  }

  result += dirName;
  return result;
} 

}


//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool ContentCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
  core::ContentsOperationResult result = GetCore().GetFileDescriptions();

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    if (EnterOperationKey())
    {
      result = GetCore().GetFileDescriptions();
    }
    else 
    {
      return true;
    }
  }

  if (result.opResult == core::OperationResult::Ok)
  {
    std::sort(result.descs.begin(), result.descs.end(), 
    [](const core::FileDescription & left, const core::FileDescription & right) 
    {
      if (left.dirName.empty() || right.dirName.empty())
      {
        if (left.dirName.empty() && !right.dirName.empty())
          return false;
        else if (!left.dirName.empty() && right.dirName.empty())
          return true;
      }
      else if (left.dirName != right.dirName)
      {
        return left.dirName < right.dirName;
      }

      if (left.name == right.name)
      {
        return left.version < right.version;
      }
   
      return left.name < right.name;
    });
        
    int verColumnWidth = std::wstring(c_versionColumnHeader).size() + c_columnGap;
    int fileNameColumnWidth = std::wstring(c_fileNameColumnHeader).size() + c_columnGap;;
   
    for (auto && nextDesc : result.descs)
    {
      std::wstring verAsStr = std::to_wstring(nextDesc.version);

      if (verAsStr.size() > verColumnWidth)
      {
        verColumnWidth = verAsStr.size() + c_columnGap;
      }

      if (nextDesc.name.size() > fileNameColumnWidth)
      {
        fileNameColumnWidth = nextDesc.name.size() + c_columnGap;  
      }
    }

    if (!result.descs.empty())
    {
      std::wstring headerLine = MakeString(c_versionColumnHeader, c_fileNameColumnHeader, 
        c_dirColumnHeader, verColumnWidth, fileNameColumnWidth);
      AddSuccessMessageStrings({headerLine});
    }
    else
    {
      AddSuccessMessageStrings({L"Sanctum is empty"});
    }

    for (auto && nextDesc : result.descs)
    {
      std::wstring verAsStr = std::to_wstring(nextDesc.version);
      std::wstring nextListLine = MakeString(verAsStr, nextDesc.name, 
        nextDesc.dirName, verColumnWidth, fileNameColumnWidth);
      AddSuccessMessageStrings({nextListLine});
    }
    
    return true;
  }
  else
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
  }
    
  return false;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring ContentCommand::GetName() const
{
  return L"cnt";
}

}