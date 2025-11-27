#include "PurgeCommand.h"
#include "IfSanctumCore.h"
#include "ConsoleTable.h"

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
PurgeCommand::PurgeCommand(core::IfSanctumCore & core)
  : Command(core)
{
}


//----------------------------------------------------------
/*
  Получить цель очистки по параметрам команды
*/
//--- 
core::PurgeTarget PurgeCommand::GetPurgeTarget(const std::vector<std::wstring> & params) const
{
  if (params.empty())
  {
    return {};
  }

  core::PurgeTarget purgeTarget;
  std::map<std::wstring,std::wstring> opts = GetOptions(params);

  if (opts.count(L"auto"))
  {
    purgeTarget.fileName.clear();
  }
  else
  {
    purgeTarget.fileName = params[0];

    if (purgeTarget.fileName.substr(0,1) == L"-")
    {
      purgeTarget.fileName.clear();
    }

    if (opts.count(L"v"))
    {
      purgeTarget.version = std::stoi(opts[L"v"]);
    }
    else if (opts.count(L"all"))
    {
      purgeTarget.isAllVersions = true;
    }
  }
 
  return purgeTarget;
}



//----------------------------------------------------------
/*
  Запустить команду
*/
//--- 
bool PurgeCommand::Run(const std::vector<std::wstring> & params)
{
  ClearMessages();
  std::map<std::wstring,std::wstring> opts = GetOptions(params);

  if (opts.count(L"fatality"))  // физическая зачистка помеченных файлов
  {
    return Purge();
  }
  else if (!params.empty() && opts.count(L"u")) // вернуть файлы в строй (снять метку)
  {
    return MarkFilesAsActive(params);
  }
  else if (!params.empty())
  {
    return MarkFilesAsPurged(params);
  }

  AddFailMessageStrings({L"Files for purge not found"});
  return false;
}


//----------------------------------------------------------
/*
  Прочистить файлы с меткой на очистку
  Файлы физически удаляются из хранилища
*/
//--- 
bool PurgeCommand::Purge()
{
  core::PurgeResult result = GetCore().Purge(core::OperationMode::Scouting);
  std::string operationKey;  

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    if (EnterOperationKey() == EnterKeyResult::Ok)
    {
      operationKey = GetCore().GetOperationKey();
      result = GetCore().Purge(core::OperationMode::Scouting);
    }
    else 
    {
      return true;
    }
  }

  if (result.opResult == core::OperationResult::Ok)
  {
    ConsoleTable table({c_versionColumnHeader, c_fileNameColumnHeader, c_dirColumnHeader});
  
    for (auto && nextDesc : result.purgedFiles)
    {
      table.AddLine({std::to_wstring(nextDesc.version), nextDesc.name, nextDesc.dirName});
    }

    table.TurnOnUnderSeparator();

    if (Confirm(table.GetPrintStrings(), L"Files will be purged. Proceed?"))
    {
      GetCore().SetOperationKey(operationKey);
      result = GetCore().Purge(core::OperationMode::Action);

      if (result.opResult == core::OperationResult::Ok)
      {
        AddSuccessMessageStrings({L"Files purged succesfully"});
      }
      else
      {
        Command::MakeMessagesForNegativeResult(result.opResult);
        return false;
      }
    }
  }
  else if (result.opResult == core::OperationResult::UncommitedChanges)
  {
    AddFailMessageStrings({L"Sanctum has uncommited changes"});
    return false;
  }
  else
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
    return false;
  }

  return true;
}


//----------------------------------------------------------
/*
  Снять с файлов метку будущей очистки
  \param params опции команды
*/
//--- 
bool PurgeCommand::MarkFilesAsActive(const std::vector<std::wstring> & params)
{
  core::PurgeTarget purgeTarget = GetPurgeTarget(params);
  core::PurgeResult result = GetCore().MarkFilesAsActive(core::OperationMode::Scouting, purgeTarget);
  std::string operationKey;  

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    if (EnterOperationKey() == EnterKeyResult::Ok)
    {
      operationKey = GetCore().GetOperationKey();
      result = GetCore().MarkFilesAsActive(core::OperationMode::Scouting, purgeTarget);
    }
    else 
    {
      return true;
    }
  }

  if (IsKeyError(result.opResult))
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
    return false;
  }
  else if (result.purgedFiles.empty())
  {
    AddSuccessMessageStrings({L"Files for purge not found"});
  } 
  else if (result.opResult == core::OperationResult::Ok)
  {
    ConsoleTable table({c_versionColumnHeader, c_fileNameColumnHeader, c_dirColumnHeader});
  
    for (auto && nextDesc : result.purgedFiles)
    {
      table.AddLine({std::to_wstring(nextDesc.version), nextDesc.name, nextDesc.dirName});
    }

    table.TurnOnUnderSeparator();

    if (Confirm(table.GetPrintStrings(), L"Files will be activated. Proceed?"))
    {
      GetCore().SetOperationKey(operationKey);
      result = GetCore().MarkFilesAsActive(core::OperationMode::Action, purgeTarget);

      if (result.opResult == core::OperationResult::Ok)
      {
        AddSuccessMessageStrings({L"Files activated successfully"});
      }
      else
      {
        Command::MakeMessagesForNegativeResult(result.opResult);
        return false;
      }
    }
  }
  else
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
    return false;
  }
  
  return true;
}


//----------------------------------------------------------
/*
  Пометить файлы для будущей очистки
*/
//--- 
bool PurgeCommand::MarkFilesAsPurged(const std::vector<std::wstring> & params)
{
  core::PurgeTarget purgeTarget = GetPurgeTarget(params);
  core::PurgeResult result = GetCore().MarkFilesAsPurged(core::OperationMode::Scouting, purgeTarget);
  std::string operationKey;  

  if (result.opResult == core::OperationResult::KeyRequired)
  {
    if (EnterOperationKey() == EnterKeyResult::Ok)
    {
      operationKey = GetCore().GetOperationKey();
      result = GetCore().MarkFilesAsPurged(core::OperationMode::Scouting, purgeTarget);
    }
    else 
    {
      return true;
    }
  }

  if (IsKeyError(result.opResult))
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
    return false;
  }
  else if (result.purgedFiles.empty())
  {
    AddSuccessMessageStrings({L"Files for purge not found"});
  }
  else if (result.opResult == core::OperationResult::Ok)
  {
    ConsoleTable table({c_versionColumnHeader, c_fileNameColumnHeader, c_dirColumnHeader});
  
    for (auto && nextDesc : result.purgedFiles)
    {
      table.AddLine({std::to_wstring(nextDesc.version), nextDesc.name, nextDesc.dirName});
    }

    table.TurnOnUnderSeparator();

    if (Confirm(table.GetPrintStrings(), L"Files will be marked for future purge. Proceed?"))
    {
      GetCore().SetOperationKey(operationKey);
      result = GetCore().MarkFilesAsPurged(core::OperationMode::Action, purgeTarget);

      if (result.opResult == core::OperationResult::Ok)
      {
        AddSuccessMessageStrings({L"Files marked for future purge successfully"});
      }
      else
      {
        Command::MakeMessagesForNegativeResult(result.opResult);
        return false;
      }
    }
  }
  else
  {
    Command::MakeMessagesForNegativeResult(result.opResult);
    return false;
  }
  
  return true;
}


//----------------------------------------------------------
/*
  Получить название команды
*/
//--- 
const std::wstring PurgeCommand::GetName() const
{
  return L"purge";
}


//----------------------------------------------------------
/*
  Получить краткую информацию о команде
*/
//--- 
std::vector<std::wstring> PurgeCommand::GetSummaryInfo() const
{
  std::vector<std::wstring> result;
  result.push_back(L"Формат: purge [fileOrDirName] [-u] [-auto] [-fatality] [-v int] [-all]");
  result.push_back(L"Служит для:");
  result.push_back(L"1) расстановки на файлы меток для будущей физической очистки");
  result.push_back(L"2) снятия с файлов меток очистки");
  result.push_back(L"3) физической очистки хранилища от помеченных файлов");
  return result;
}


//----------------------------------------------------------
/*
  Детальную информацию о команде
*/
//--- 
std::vector<std::wstring> PurgeCommand::GetDetailInfo() const
{
  std::vector<std::wstring> result;
  result.push_back(L"[fileOrDirName] имя файла\\директории или фрагмент имени");
  result.push_back(L"Варианты (по приоритету):");
  result.push_back(L"1) пустое - любой файл в хранилище");
  result.push_back(L"2) полный путь к файлу в хранилище");
  result.push_back(L"3) полный путь к директории в хранилище");
  result.push_back(L"4) фрагмент имени файла");
  result.push_back(L"[-u] снять метку очистки");
  result.push_back(L"[-auto] автоматический фильтр версий файлов при редактировании меток");
  result.push_back(L"При установке меток будут помечаться только неактуальные версии файлов");
  result.push_back(L"При снятии меток будут активироваться только актуальные версии файлов");
  result.push_back(L"[-v] редактирование метки для конкретной версии файлов");
  result.push_back(L"[-all] редактирование метки для всех версий файлов");
  result.push_back(L"[-fatality] физически прочистить хранилище от помеченных файлов");
  result.push_back(L"Не требует никаких дополнительных опций, они будут проигнорированы");

  return result;
}

}