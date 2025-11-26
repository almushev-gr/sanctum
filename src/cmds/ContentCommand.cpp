#include "ContentCommand.h"
#include "Command.h"
#include "IfSanctumCore.h"
#include "ConsoleTable.h"
#include <string>
#include <algorithm>


namespace 
{
  constexpr wchar_t c_versionColumnHeader[] = L"Ver";
  constexpr wchar_t c_purgeCandidateColumnHeader[] = L"Purge";
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
  std::map<std::wstring, std::wstring> opts = Command::GetOptions(params);

  core::ContentsOperationResult result;

  if (opts.count(L"drop"))
  {
    GetCore().ClearContents();
    result.opResult = core::OperationResult::Ok;
    AddSuccessMessageStrings({L"ContentsTable is clear"});
    return true;
  }

  result = GetFileDescriptions(opts);

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    EnterKeyResult enterResult = EnterOperationKey();

    if (enterResult == EnterKeyResult::Ok)
    {
      result = GetFileDescriptions(opts);
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
    ConsoleTable table(GetTableHeaders(params));
        
    for (auto && nextDesc : result.descs)
    {
      table.AddLine(GetTableLine(params, nextDesc));
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
  else if (result.opResult == core::OperationResult::FileProcessFail)
  {
    AddFailMessageStrings({L"Cant get contents. Perhaps wrong key entered"});
  }
  else
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
  }
    
  return false;
}


//----------------------------------------------------------
/*
  Получить описания файлов в зависимости от ключей
*/
//--- 
core::ContentsOperationResult ContentCommand::GetFileDescriptions(const std::map<std::wstring, std::wstring> & opts) const
{
  if (opts.count(L"commit"))
  {
    return GetCore().GetCommitFileDescriptions();
  }

  return GetCore().GetFileDescriptions();
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

  std::map<std::wstring, std::wstring> opts = Command::GetOptions (params);

  if (opts.count(L"mv")) // показать файлы только с максимальной версией
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
  
  if (opts.count(L"f") && !opts[L"f"].empty()) // показать только файлы, в имени которых содержится подстрока
  {
    result.descs.erase(std::remove_if(result.descs.begin(), result.descs.end(),
                                 [&opts](const core::FileDescription & nextDesc) 
                                 {  
                                  return nextDesc.name.find(opts[L"f"]) == std::wstring::npos;
                                 }),
                  result.descs.end());
  }

  if (opts.count(L"pc")) // показать только кандидатов на очистку
  {
    result.descs.erase(std::remove_if(result.descs.begin(), result.descs.end(),
                                 [&opts](const core::FileDescription & nextDesc) 
                                 {  
                                  return !nextDesc.isPurgeCandidate;
                                 }),
                  result.descs.end());
  }

  if (opts.count(L"npc")) // показать только не-кандидатов на очистку
  {
    result.descs.erase(std::remove_if(result.descs.begin(), result.descs.end(),
                                 [&opts](const core::FileDescription & nextDesc) 
                                 {  
                                  return nextDesc.isPurgeCandidate;
                                 }),
                  result.descs.end());
  }
}


//----------------------------------------------------------
/*
  Получить заголовки для таблицы с файлами
*/
//--- 
std::vector<std::wstring> ContentCommand::GetTableHeaders(const std::vector<std::wstring> & params)
{
  std::map<std::wstring, std::wstring> opts = Command::GetOptions(params);

  if (opts.count(L"pch")) // показывать маркер очистки
  {
    return {c_versionColumnHeader, c_purgeCandidateColumnHeader, c_fileNameColumnHeader, c_dirColumnHeader};
  }

  return {c_versionColumnHeader, c_fileNameColumnHeader, c_dirColumnHeader};
}


//----------------------------------------------------------
/*
  Получить строку для таблицы
*/
//--- 
std::vector<std::wstring> ContentCommand::GetTableLine(const std::vector<std::wstring> & params, const core::FileDescription & fileDesc)
{
  std::map<std::wstring, std::wstring> opts = Command::GetOptions(params);

  if (opts.count(L"pch"))
  {
    std::wstring version = std::to_wstring(fileDesc.version);
    std::wstring purgeCandidate = fileDesc.isPurgeCandidate ? L"Purge" : L"In use";

    return {version, purgeCandidate, fileDesc.name, fileDesc.dirName};
  }

  return {std::to_wstring(fileDesc.version), fileDesc.name, fileDesc.dirName};
}

}