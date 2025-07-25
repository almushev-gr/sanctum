#include "SanctumDefaultCore.h"
#include "DefaultEncrypter.h"
#include "IfEncrypter.h"
#include "IfSanctumCore.h"
#include <pugixml/pugixml.hpp>
#include <DefaultEncrypter.h>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <functional>
#include <windows.h>


namespace 
{
  constexpr wchar_t c_sanctumDefaultName[] = L"sanctum";
}


namespace sanctum::core
{

//----------------------------------------------------------
/*
  
*/
//---
DefaultCore::DefaultCore()
  : m_encrypter(sanctum::encrypter::GetEncrypter())
  , m_workDir(std::filesystem::current_path())
  , m_workFile()
  , m_sanctumDir(std::filesystem::current_path())
  , m_sanctumName(c_sanctumDefaultName)
  , m_sanctumPath()
  , m_contentsTable()
  , m_coreKeyHash(std::hash<std::string>{}("defpass"))
{
  LoadConfig();
  m_sanctumPath = m_sanctumDir / m_sanctumName;
}


//----------------------------------------------------------
/*
  Получить полный путь к фантому
*/
//---
std::filesystem::path DefaultCore::GetFantomPath() const
{
  std::wstring fantomName = m_sanctumName.stem().wstring() + L"_fantom" + m_sanctumName.extension().wstring();
  return m_sanctumDir / fantomName;
}


//----------------------------------------------------------
/*
  Получить на хранилище с наиболее актуальным состоянием,
  т.е. содержащее все текущее изменения.
  Если фантом опережает основное хранилище, то предпочтение
  отдается фантому
*/
//---
std::filesystem::path DefaultCore::GetRelevantPath() const
{
  if (std::filesystem::exists(GetFantomPath()))
  {
    return GetFantomPath();
  }

  return m_sanctumPath;
}


//----------------------------------------------------------
/*
  Задать рабочую директорию
*/
//---
OperationResult DefaultCore::SetWorkDir(const std::wstring & dirFullPath)
{
  if (dirFullPath.empty()) 
  {
    m_workDir = std::filesystem::current_path();
    return OperationResult::Ok;
  }

  if (!std::filesystem::exists(dirFullPath))
  {
    return OperationResult::NoSuchFileOrDir;
  }

  m_workDir = dirFullPath;
  return OperationResult::Ok;
}


//----------------------------------------------------------
/*
  Задать путь к рабочему файлу (полный путь)
*/
//---
OperationResult DefaultCore::SetWorkFile(const std::wstring & fileFullPath)
{
  if (fileFullPath.empty()) 
  {
    m_workFile.clear();
    return OperationResult::Ok;
  }

  if (!std::filesystem::exists(fileFullPath))
  {
    return OperationResult::NoSuchFileOrDir;
  }

  m_workFile = fileFullPath;
  return OperationResult::Ok;
}


//----------------------------------------------------------
/*
  Взять файл из хранилища
*/
//---
FileOperationResult DefaultCore::Get(const std::wstring & getPath)
{
  FileOperationResult result;

  if (getPath.empty())
  {
    result.opResult = OperationResult::NoSuchFileOrDir;
    return result;
  }

  OperationResult checkKeyResult = CheckKey();

  if (checkKeyResult != OperationResult::Ok)
  {
    m_operationKey.clear();
    result.opResult = checkKeyResult;
    return result;
  }
 
  std::filesystem::path filePath(getPath);
  std::filesystem::path fileOrDirName = filePath.filename();
  std::filesystem::path dirInSanctum = filePath.parent_path();
  result.opResult = OperationResult::Ok;

  if (GetContentsTable().IsFileExist(dirInSanctum, fileOrDirName))
  {
    result = GetFile(dirInSanctum, fileOrDirName);
  }
  else if (GetContentsTable().IsDirExist(getPath))
  {
    result = GetDir(getPath);
  }
  else  
  {
    std::set<std::filesystem::path> dirs = GetContentsTable().GetDirsContainsString(getPath);
    std::set<std::filesystem::path> files = GetContentsTable().GetFilesContainsString(getPath);
    size_t varCount = dirs.size() + files.size();
    
    if (varCount == 0)
    {
      result.problemFiles.push_back(getPath);
      result.opResult = OperationResult::NoSuchFileOrDir;
    }
    else if (varCount == 1 && !dirs.empty())
    {
      result = GetDir(dirs.begin()->wstring());
    }
    else if (varCount == 1 && !files.empty())
    {
      result = GetFile(files.begin()->parent_path(), files.begin()->filename());
    }
    else // неоднозначность. нужно уточнить у пользователя
    {
      std::transform(dirs.begin(), dirs.end(), std::back_inserter(result.ambiguousFiles), 
        [](const std::filesystem::path & nextPath) { return nextPath.wstring();});
      std::transform(files.begin(), files.end(), std::back_inserter(result.ambiguousFiles), 
        [](const std::filesystem::path & nextPath) { return nextPath.wstring();});

      result.opResult = OperationResult::AmbiguousInput;
    }
  }

  m_operationKey.clear();
  return result;
}


//----------------------------------------------------------
/*
  Получить директорию из хранилища
*/
//---
FileOperationResult DefaultCore::GetDir(const std::wstring & dirPath)
{
  FileOperationResult result;
  result.opResult = OperationResult::NoSuchFileOrDir;

  for (auto && nextDesc : GetContentsTable().GetFilesInDir(dirPath))
  {
    result = GetFile(nextDesc.dirName, nextDesc.name);

    if (result.opResult != OperationResult::Ok)
    {
      break;
    }
  }

  return result;
}


//----------------------------------------------------------
/*
  Получить файл из хранилища
*/
//---
FileOperationResult DefaultCore::GetFile(const std::filesystem::path & dirInSanctum, const std::filesystem::path & fileName)
{
  FileOperationResult result;
  std::filesystem::path pathInSanctum = dirInSanctum / fileName;
  std::filesystem::path finalPath = m_workDir / pathInSanctum;

  if (std::filesystem::exists(finalPath))
  {
    result.problemFiles.push_back(pathInSanctum.wstring());
    result.opResult = OperationResult::FileAlreadyExist;
    return result;
  }

  std::optional<FileDescription> actualFileDesc = GetContentsTable().GetActual(dirInSanctum, fileName);

  if (!actualFileDesc)
  {
    result.problemFiles.push_back(pathInSanctum.wstring());
    result.opResult = OperationResult::NoSuchFileOrDir;
    return result;
  }

  std::ifstream sanctumFile(GetRelevantPath().string(), std::ios::binary);

  if (!sanctumFile.is_open())
  {
    result.opResult = OperationResult::NoSanctum;
    return result;
  }

  sanctumFile.unsetf(std::ios::skipws);
  sanctumFile.seekg(actualFileDesc->offset);
  FileInsideSanctum fileInSanctum;

  if (fileInSanctum.ReadFrom(sanctumFile, FileInsideSanctum::FileReadMode::Full, *m_encrypter, GetKey()))
  {
    if (fileInSanctum.SaveTo(m_workDir))
    {
      result.opResult = OperationResult::Ok;
    }
    else
    {
      result.problemFiles.push_back(pathInSanctum.wstring());
      result.opResult = OperationResult::FileProcessFail;
    }
  }
  else
  {
    result.problemFiles.push_back(pathInSanctum.wstring());
    result.opResult = OperationResult::FileProcessFail;
  }
  
  sanctumFile.close();
  return result;
}



//----------------------------------------------------------
/*
  Поместить файл в хранилище
*/
//---
FileOperationResult DefaultCore::Put(const std::wstring & path)
{
  FileOperationResult result;
  OperationResult checkKeyResult = CheckKey();

  if (checkKeyResult != OperationResult::Ok)
  {
    m_operationKey.clear();
    result.opResult = checkKeyResult;
    return result;
  }

  result.opResult = OperationResult::Ok;
  std::filesystem::path putPath(path);

  if (putPath.empty()) // разместить рабочий файл
  {
    if (m_workFile.empty())
    {
      result.opResult = OperationResult::NoWorkFile;
    }
    else 
    {
      result = PutFileByAbsPath(m_workFile);
    }
  }
  else if (putPath.is_absolute())
  {
    if (std::filesystem::is_directory(putPath))
    {
      result = PutDirByAbsPath(putPath);
    }
    else 
    {
      result = PutFileByAbsPath(putPath);
    }
  }
  else // относительный путь в рабочей директории
  {
    std::filesystem::path absPath = m_workDir / putPath;

    if (std::filesystem::is_directory(absPath))
    {
      result = PutDirByAbsPath(absPath);
    }
    else if (putPath.parent_path().empty()) // просто имя файла
    {
      std::vector<std::filesystem::path> foundFiles = FindFilesInWorkDir(putPath.filename());
     
      if (foundFiles.empty())
      {
        result.problemFiles.push_back(putPath);
        result.opResult = OperationResult::NoSuchFileOrDir;
      }
      else if (foundFiles.size() == 1)
      {
        result = PutFileByAbsPath(foundFiles[0]);
      }
      else 
      {
        std::transform(foundFiles.begin(), foundFiles.end(), std::back_inserter(result.ambiguousFiles), 
        [](const std::filesystem::path & nextPath) { return nextPath.wstring();});
        result.opResult = OperationResult::AmbiguousInput;
      }
    }
    else
    {
      result = PutFileByAbsPath(absPath);
    }
  }
 
  m_operationKey.clear();
  return result;
}


//----------------------------------------------------------
/*
  Проверка ключа шифрации
*/
//---
OperationResult DefaultCore::CheckKey() const
{
  OperationResult result = OperationResult::Ok;

  switch (m_encrypter->GetKeyPolicy())
  {
    case sanctum::encrypter::KeyPolicy::KeyForCall:
    {
      if (GetKey().empty())
      {
        result = OperationResult::KeyRequired;
      }
      else if (std::hash<std::string>{}(GetKey()) != m_coreKeyHash)
      {
        result = OperationResult::InvalidKey;
      }
      else 
      {
        result = CheckDecryption();      
      }
    }
    break;

    case sanctum::encrypter::KeyPolicy::KeyForEncryption:
    {
      if (GetKey().empty())
      {
        result = OperationResult::KeyRequired;
      }
      else
      {
        result = CheckDecryption();
      }
    }
    break;

    case sanctum::encrypter::KeyPolicy::NoKey:
    break;
  }
  
  return result;
}


//----------------------------------------------------------
/*
  Проверить дешифрацию
  Читает имя шифратора из заголовка первого файла
*/
//---
OperationResult DefaultCore::CheckDecryption() const
{
  // хранилища пока нет. Так что расшифровывать нечего
  if (!std::filesystem::exists(GetRelevantPath()))
  {
    return OperationResult::Ok;
  }

   std::ifstream sanctumFile(GetRelevantPath().string(), std::ios::binary);

  if (!sanctumFile.is_open())
  {
    return OperationResult::NoSanctum;
  }

  sanctumFile.unsetf(std::ios::skipws);
  FileInsideSanctum firstFile;
  firstFile.ReadFrom(sanctumFile, FileInsideSanctum::FileReadMode::HeaderOnly, *m_encrypter, GetKey());
  sanctumFile.close();

  if (firstFile.GetEncName() == m_encrypter->GetName())
  {
    return OperationResult::Ok;
  }

  if (m_encrypter->GetKeyPolicy() == encrypter::KeyPolicy::KeyForCall)
  {
    return OperationResult::WrongEncrypter;
  }
  
  return OperationResult::WrongKeyOrEncrypter;
}


//----------------------------------------------------------
/*
  Получить ключ шифрации
*/
//---
std::string DefaultCore::GetKey() const
{
  if (!m_permanentKey.empty())
  {
    return m_permanentKey;
  }
  else if (!m_operationKey.empty())
  {
    return m_operationKey;
  }

  return "";
}


//----------------------------------------------------------
/*
  Найти файлы по имени в рабочей директории
*/
//---
std::vector<std::filesystem::path> DefaultCore::FindFilesInWorkDir(const std::filesystem::path & fileName)
{
  std::vector<std::filesystem::path> foundFiles;
      
  try 
  {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(m_workDir)) 
    {
      if (!std::filesystem::is_regular_file(entry))
      {
        continue;
      }

      if (fileName.has_extension())
      {
        if (fileName == entry.path().filename())
        {
          foundFiles.push_back(entry.path());
        }
      }
      else
      {
        if (fileName == entry.path().stem())
        {
          foundFiles.push_back(entry.path());
        }
      }
    }
  } 
  catch (const std::filesystem::filesystem_error& err) 
  {
    foundFiles.clear();
  }

  return foundFiles;
}


//----------------------------------------------------------
/*
  Поместить файл в хранилище по абсолютному пути
*/
//---
FileOperationResult DefaultCore::PutFileByAbsPath(const std::filesystem::path & putPath)
{
  FileOperationResult result;
  result.opResult = OperationResult::Ok;

  if (std::filesystem::exists(putPath))
  {
    std::filesystem::path dirInSanctum = GetDirInSanctum(putPath);
    FileInsideSanctum putFile;
    putFile.SetFullPath(putPath);
    putFile.SetDirInSanctum(dirInSanctum.wstring());
    int fileVersion = GetContentsTable().GetFileNextVersion(dirInSanctum.wstring(), putPath.filename().wstring());
    putFile.SetVersion(fileVersion);
    std::vector<FileInsideSanctum> putFiles{putFile};
    result = PutFiles(putFiles, PutFileMethod::Append);
  }
  else 
  {
    result.problemFiles.push_back(putPath.wstring());
    result.opResult = OperationResult::NoSuchFileOrDir;
  }

  return result;
}


//----------------------------------------------------------
/*
  Поместить директорию в хранилище по абсолютному пути
*/
//---
FileOperationResult DefaultCore::PutDirByAbsPath(const std::filesystem::path & putPath)
{
  FileOperationResult result;
 
  if (!std::filesystem::exists(putPath))
  {
    result.problemFiles.push_back(putPath.wstring());
    result.opResult = OperationResult::NoSuchFileOrDir;
    return result;
  }

  result.opResult = OperationResult::Ok;
  std::filesystem::path nextFile;
    
  try 
  {
    std::vector<FileInsideSanctum> filesInSanctum;
    std::filesystem::path dirInSanctum = GetDirInSanctum(putPath);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(putPath)) 
    {
      if (std::filesystem::is_regular_file(entry)) 
      {
        nextFile = entry.path();
        std::wstring insideDirPath = GetFileDirUpTo(nextFile.wstring(), putPath.filename().wstring());
        std::wstring dirInSanctumFullPath = dirInSanctum / insideDirPath;
        int version = GetContentsTable().GetFileNextVersion(dirInSanctumFullPath, entry.path().filename());
        filesInSanctum.emplace_back(entry.path().wstring(), dirInSanctumFullPath, version);
      }
    }

    result = PutFiles(filesInSanctum, PutFileMethod::Append);

    if (result.opResult == OperationResult::Ok)
    {
      result.opResult = RemoveFromDisk({putPath.wstring()});
    }
  } 
  catch (const std::filesystem::filesystem_error& err) 
  {
    if (nextFile.empty())
    {
      result.problemFiles.push_back(L"Unknown file");
    }
    else
    {
      result.problemFiles.push_back(nextFile);
    }
    
    result.opResult = OperationResult::FileProcessFail;
  }

  return result;
}


//----------------------------------------------------------
/*
  Получить директорию в хранилище по абсолютному пути
*/
//---
std::filesystem::path DefaultCore::GetDirInSanctum(const std::filesystem::path & absPath) const
{
  std::filesystem::path dirInSanctum;
  std::filesystem::path pathInWorkDir = std::filesystem::relative(absPath, m_workDir);  

  if (!pathInWorkDir.empty())
  {
    if (*pathInWorkDir.begin() == L"..") // файл\директория вне рабочей директории
    {
      dirInSanctum.clear();
    }
    else
    {
      dirInSanctum = pathInWorkDir.parent_path();
    }
  }

  return dirInSanctum;
}


//----------------------------------------------------------
/*
  Получить описания файла
*/
//---
ContentsOperationResult DefaultCore::GetFileDescriptions()
{
  ContentsOperationResult result;
  OperationResult checkKeyResult = CheckKey();

  if (checkKeyResult != OperationResult::Ok)
  {
    m_operationKey.clear();
    result.opResult = checkKeyResult;
    return result;
  }
   
  result.opResult = OperationResult::Ok;

  if (GetContentsTable().IsEmpty()) // возможно был введен не тот ключ
  {
    result.opResult = GetContentsTable().Update(GetRelevantPath(), *m_encrypter, GetKey());
    result.descs = GetContentsTable().GetDescriptions();
  }
  else 
  {
    result.descs = GetContentsTable().GetDescriptions();
  }
  
  m_operationKey.clear();
  return result;
}


//----------------------------------------------------------
/*
  Очистить закешированное оглавление хранилища
*/
//---
void DefaultCore::ClearContents()
{
  m_contentsTable.reset();
}


//----------------------------------------------------------
/*
  Задать имя файла хранилища
  Допустимо, что хранилища с таким именем еще не существует
  В таком случае его фантом будет создан при размещении первого файла
*/
//---
OperationResult DefaultCore::SetSanctumName(const std::wstring & name)
{
  m_sanctumName = name;
  m_sanctumPath = m_sanctumDir / m_sanctumName;
  m_contentsTable.reset();
    
  return OperationResult::Ok;
}


//----------------------------------------------------------
/*
  Задать директорию хранилища
*/
//---
OperationResult DefaultCore::SetSanctumDir(const std::wstring & dirFullPath)
{
  if (std::filesystem::exists(dirFullPath))
  {
    m_sanctumDir = dirFullPath;
    m_sanctumPath = m_sanctumDir / m_sanctumName;
    m_contentsTable.reset();
    return OperationResult::Ok;
  }

  return OperationResult::NoSuchFileOrDir;
}


//----------------------------------------------------------
/*
  Получить директорию хранилища
*/
//---
std::wstring DefaultCore::GetSanctumDir() const
{
  return m_sanctumDir.wstring();
}


//----------------------------------------------------------
/*
  Получить рабочую директорию
*/
//---
std::wstring DefaultCore::GetWorkDir() const
{
  return m_workDir.wstring();
}
 

//----------------------------------------------------------
/*
  Получить часть пути к файлу до определенной директории
*/
//---
std::wstring DefaultCore::GetFileDirUpTo(const std::wstring & fullFileName, const std::wstring & dirName)
{
  std::filesystem::path fullFilePath(fullFileName);
  std::filesystem::path currentDirPath(fullFilePath.parent_path());
  std::vector<std::filesystem::path> pathElements;

  while (currentDirPath != currentDirPath.parent_path())
  {
    pathElements.push_back(currentDirPath.filename());
    currentDirPath = currentDirPath.parent_path();
  }

  std::reverse(pathElements.begin(), pathElements.end());
  auto dirPathIt = std::find(pathElements.begin(), pathElements.end(), dirName);

  if (dirPathIt != pathElements.end())
  {
    std::filesystem::path resultPath;
    resultPath = std::accumulate(dirPathIt, pathElements.end(), resultPath, [](auto p, const auto& part) {return p /= part;});
    return resultPath.wstring();
  }

  return fullFilePath.parent_path().wstring();
}


//----------------------------------------------------------
/*
  Поместить файлы в хранилище
*/
//---
FileOperationResult DefaultCore::PutFiles(std::vector<FileInsideSanctum> & filesInSanctum, PutFileMethod method)
{
  FileOperationResult result;
  std::unique_ptr<std::ofstream> fantomFile = GetFantomOutputStream(method);

  if (!fantomFile)
  {
    result.opResult = OperationResult::NoSanctum;
    return result;
  }

  result.opResult = OperationResult::Ok;
  fantomFile->seekp(0, std::ios_base::end);
  
  for (auto && nextFile : filesInSanctum)
  {
    if (nextFile.WriteTo(*fantomFile, *m_encrypter, GetKey()))
    {
      FileDescription newDesc;
      newDesc.version = nextFile.GetVersion();
      newDesc.name = nextFile.GetName();
      newDesc.dirName = nextFile.GetDirName();
      newDesc.offset = nextFile.GetOffset();
      GetContentsTable().AddFile(newDesc);
    }
    else
    {
      result.problemFiles.push_back(nextFile.GetFullPath());
      result.opResult = OperationResult::FileProcessFail;
      break;
    }
  }

  if (fantomFile->fail())
  {
    result.opResult = OperationResult::NoSanctum;
  }

  fantomFile->close();

  if (result.opResult == OperationResult::Ok)
  {
    std::vector<std::wstring> pathsToRemove;
    
    for (auto && nextFile : filesInSanctum)
    {
      pathsToRemove.push_back(nextFile.GetFullPath());
    }

    result.opResult = RemoveFromDisk(pathsToRemove);
  }

  return result;
}


//----------------------------------------------------------
/*
  Зафиксировать изменения фантома
*/
//---
OperationResult DefaultCore::Commit()
{
  if (m_sanctumPath == GetRelevantPath())
  {
    return OperationResult::NoSanctum;
  }
 
  OperationResult result = OperationResult::Ok;

  if (std::filesystem::exists(m_sanctumPath))
  {
    result = RemoveFromDisk({m_sanctumPath.wstring()});
  }

  if (result == OperationResult::Ok)
  {
    try
    {
      std::filesystem::rename(GetRelevantPath(), m_sanctumPath);
    } 
    catch (const std::filesystem::filesystem_error& e)
    {
      result = OperationResult::RenameFileError;  
    }
  }
   
  return result;
}


//----------------------------------------------------------
/*
  Получить фантомный файл хранилища
*/
//---
std::unique_ptr<std::ofstream> DefaultCore::GetFantomOutputStream(PutFileMethod method)
{
  std::ios_base::openmode mode = std::ios::binary;

  if (method == PutFileMethod::Append)
  {
    mode = mode | std::ios::app;

    if (!std::filesystem::exists(GetFantomPath()) && std::filesystem::exists(m_sanctumPath))
    {
      try 
      {
        std::filesystem::copy(m_sanctumPath, GetFantomPath());
      }
      catch (const std::filesystem::filesystem_error& err)
      {
        return nullptr;
      }
    }
  }

  std::unique_ptr<std::ofstream> fantomStream = std::make_unique<std::ofstream>(GetFantomPath().string().c_str(), mode);
  
  if (fantomStream->is_open())
  {
    return fantomStream;
  }
  
  return nullptr;
}


//----------------------------------------------------------
/*
  Сохранить конфигурацию ядра
*/
//---
bool DefaultCore::SaveConfig() const
{
  pugi::xml_document doc;

  pugi::xml_node cfgRootNode = doc.append_child(L"SanctumCfg");
  pugi::xml_node cfgDirNode = cfgRootNode.append_child(L"SanctumDir");

  if (m_sanctumDir == std::filesystem::current_path())
  {
    cfgDirNode.append_child(pugi::node_pcdata).set_value(L"Default");
  }
  else
  {
    cfgDirNode.append_child(pugi::node_pcdata).set_value(m_sanctumDir.wstring().c_str());
  }
  
  pugi::xml_node cfgWorkDirNode = cfgRootNode.append_child(L"WorkDir");
  
  if (m_workDir == std::filesystem::current_path())
  {
    cfgWorkDirNode.append_child(pugi::node_pcdata).set_value(L"Default");
  }
  else
  {
    cfgWorkDirNode.append_child(pugi::node_pcdata).set_value(m_workDir.wstring().c_str());
  }

  pugi::xml_node encNode = cfgRootNode.append_child(L"Encrypter");

  if (m_outsideEncrypterPath.empty())
  {
    encNode.append_child(pugi::node_pcdata).set_value(L"Default");
  }
  else
  {
    encNode.append_child(pugi::node_pcdata).set_value(m_outsideEncrypterPath.wstring().c_str());
  }

  if (doc.save_file("sanctumcfg.xml")) 
  {
    return true;
  }
  
  return false;
}


//----------------------------------------------------------
/*
  Загрузить конфигурацию ядра из внешнего xml-файла
*/
//---
void DefaultCore::LoadConfig()
{
  pugi::xml_document doc;

  if (!doc.load_file("sanctumcfg.xml"))
  {
    return;
  }

  pugi::xml_node cfgRootNode = doc.child(L"SanctumCfg");

  if (!cfgRootNode)
  {
    return;
  }

  pugi::xml_node cfgDirNode = cfgRootNode.child(L"SanctumDir");
  std::wstring cfgNodeValue;

  if (cfgDirNode)
  {
    cfgNodeValue = cfgDirNode.child_value();

    if (cfgNodeValue == L"Default")
    {
      m_sanctumDir = std::filesystem::current_path();
    }
    else 
    {
      m_sanctumDir = cfgNodeValue;
    }
  }

  pugi::xml_node cfgWorkDirNode = cfgRootNode.child(L"WorkDir");

  if (cfgWorkDirNode)
  {
    cfgNodeValue = cfgWorkDirNode.child_value();

    if (cfgNodeValue == L"Default")
    {
      m_workDir = std::filesystem::current_path();
    }
    else 
    {
      m_workDir = cfgNodeValue;
    }
  }

  pugi::xml_node encNode = cfgRootNode.child(L"Encrypter");

  if (encNode)
  {
    cfgNodeValue = encNode.child_value();

    if (cfgNodeValue == L"Default")
    {
      m_encrypter = sanctum::encrypter::GetEncrypter(); // уиолчательный шифратор
    }
    else
    {
      LoadEncrypter(cfgNodeValue);
    }
  }

}


//----------------------------------------------------------
/*
  Задать оперативный ключ шифрации
  Действует только для одной операции, требующей ключа
  По окончании операции обнуляется
*/
//---
void DefaultCore::SetOperationKey(const std::string & key)
{
  m_operationKey = key;
}


//----------------------------------------------------------
/*
  Удалить файлы с диска
*/
//---
OperationResult DefaultCore::RemoveFromDisk(const std::vector<std::wstring> & paths)
{
  try 
  {
    for (auto && nextPath : paths)
    {
      std::filesystem::remove_all(nextPath);
    }
  } 
  catch (std::filesystem::filesystem_error & er) 
  {
    return OperationResult::RemoveFileError;
  }

  return OperationResult::Ok;
}


//----------------------------------------------------------
/*
  Получить оглавление хранилища
  Вызывающий код должен обеспечить наличие ключа шифрации
  Фактически доступ к содержанию возможен только
  при его наличии
*/
//---
ContentsTable & DefaultCore::GetContentsTable()
{
  if (m_contentsTable)
  {
    return *m_contentsTable;
  }

  m_contentsTable = std::make_unique<ContentsTable>();
  m_contentsTable->Update(GetRelevantPath(), *m_encrypter, GetKey());
  return *m_contentsTable;
}


//----------------------------------------------------------
/*
  Загрузить сторонний шифратор
*/
//---
OperationResult DefaultCore::LoadEncrypter(const std::wstring & encPath)
{
  std::filesystem::path dllPath = encPath;

  if (!dllPath.is_absolute())
  {
    dllPath = std::filesystem::current_path() / dllPath;
  }

  if (!std::filesystem::exists(dllPath))
  {
    return OperationResult::NoSuchFileOrDir;
  }

  if (!m_outsideEncrypterPath.empty())
  {
    return OperationResult::OutsideEncrypterAlreadyLoaded;
  }

  HINSTANCE hEnc = ::LoadLibrary(dllPath.string().c_str());

  if (!hEnc)
  {
    return OperationResult::UnknownError;
  }

  m_outsideEncrypterPath = encPath;
  OperationResult result = OperationResult::UnknownError;

  FARPROC fn = ::GetProcAddress(hEnc, "GetEncrypter");

  if (fn)
  {
    sanctum::encrypter::GetEncrypterPtr getEncPtr = (sanctum::encrypter::GetEncrypterPtr)fn;
    sanctum::encrypter::IfEncrypter * encFromLib = getEncPtr();

    if (encFromLib)
    {
      m_encrypter = encFromLib;
      m_contentsTable.reset();
      result = OperationResult::Ok;
    }
  }

  if (result != OperationResult::Ok)
  {
    UnloadEncrypter();
  }

  return result;
}


//----------------------------------------------------------
/*
  Выгрузить шифратор
*/
//---
OperationResult DefaultCore::UnloadEncrypter()
{
  if (m_outsideEncrypterPath.empty())
  {
    return OperationResult::OutsideEncrypterNotLoaded;
  }

  std::string encName = m_outsideEncrypterPath.filename().string();
  HINSTANCE hEnc = ::GetModuleHandle(encName.c_str());

  if (hEnc)
  {
    if (::FreeLibrary(hEnc))
    {
      m_outsideEncrypterPath.clear();
      m_encrypter = encrypter::GetEncrypter();
      m_contentsTable.reset();
      return OperationResult::Ok;
    }
  }

  return OperationResult::UnknownError;
}


//----------------------------------------------------------
/*
  Получить имя шифратора
*/
//---
std::wstring DefaultCore::GetEncrypterName() const
{
  return m_encrypter->GetName();
}


//----------------------------------------------------------
/*
  Создать ядро хранилища
*/
//---
std::unique_ptr<IfSanctumCore> CreateSanctumCore()
{
  return std::make_unique<DefaultCore>();
}

}