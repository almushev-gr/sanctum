#include "ContentCommand.h"
#include "IfSanctumCore.h"
#include "ConsoleTable.h"
#include <string>
#include <algorithm>


namespace 
{
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

//----------------------------------------------------------
/*
  Отсортировать полученные описания файлов
*/
//--- 
void SortResultDescriptions(core::ContentsOperationResult & result)
{
  std::sort(result.descs.begin(), result.descs.end(), 
    [](const core::FileDescription & left, const core::FileDescription & right) 
  {
    if (left.dirName.empty() || right.dirName.empty())
    {
      if (left.dirName.empty() && !right.dirName.empty())
      {
        return false;
      }        
      else if (!left.dirName.empty() && right.dirName.empty())
      {
        return true;
      }
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
}

}


// todo:
// нужен флажок фильтра по имени файла -f
// хорошо бы показать общее количество файлов в хранилище


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
    SortResultDescriptions(result);
    ApplyOptionsToResult(params, result);
    ConsoleTable table({c_versionColumnHeader, c_fileNameColumnHeader, c_dirColumnHeader});
    
    for (auto && nextDesc : result.descs)
    {
      table.AddLine({std::to_wstring(nextDesc.version), nextDesc.name, nextDesc.dirName});
    }
   
    if (table.IsEmpty())
    {
      AddSuccessMessageStrings({L"Sanctum is empty"});
    }
    else
    {
      table.TurnOnUnderSeparator();
      AddSuccessMessageStrings(table.GetPrintStrings());
      AddSuccessMessageStrings({L"Total files: " + std::to_wstring(result.descs.size())});
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


//----------------------------------------------------------
/*
  Применить опции команды к результату
*/
//--- 
void ContentCommand::ApplyOptionsToResult(const std::vector<std::wstring> & params, core::ContentsOperationResult & result)
{
  if (params.empty())
  {
    return;
  }

  std::map<std::wstring, std::wstring> opts = GetOptions(params);

  if (opts.count(L"mv"))
  {
    using FileID = std::pair<std::wstring, std::wstring>;
    std::map<FileID, int> maxVersions;

    for (auto && nextDesc : result.descs)
    {
      FileID nextFileID{nextDesc.dirName, nextDesc.name};

      if (maxVersions.count(nextFileID))
      {
        if (nextDesc.version > maxVersions[nextFileID])
        {
           maxVersions[nextFileID] = nextDesc.version;
        }
      }
      else
      {
        maxVersions[nextFileID] = nextDesc.version;
      }
    }

    size_t i = 0;

    result.descs.erase(std::remove_if(result.descs.begin(), result.descs.end(),
                                 [&maxVersions](const core::FileDescription & nextDesc) 
                                 {  
                                  return nextDesc.version < maxVersions[{nextDesc.dirName, nextDesc.name}];
                                 }),
                  result.descs.end());
  }
  
  if (opts.count(L"f") && !opts[L"f"].empty())
  {
    result.descs.erase(std::remove_if(result.descs.begin(), result.descs.end(),
                                 [&opts](const core::FileDescription & nextDesc) 
                                 {  
                                  return nextDesc.name.find(opts[L"f"]) == std::wstring::npos;
                                 }),
                  result.descs.end());
  }

}

}