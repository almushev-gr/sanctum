#include "SanctumDefaultCore.h"
#include "IfSanctumCore.h"
#include <DefaultEncrypter.h>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <numeric>


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
  : m_encrypter(std::make_unique<encrypter::DefaultEncrypter>())
  , m_workDir(std::filesystem::current_path())
  , m_workFile()
  , m_sanctumDir(std::filesystem::current_path())
  , m_sanctumName(c_sanctumDefaultName)
  , m_sanctumPath()
  , m_contentsTable()
 {
  // todo: считать конфигу
  // здесь должен быть уже сформированы корректные пути к хранилищу

  m_sanctumPath = m_sanctumDir / m_sanctumName;
  m_contentsTable.Update(GetRelevantPath(), *m_encrypter);
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
FileOperationResult DefaultCore::Get(const std::wstring & path)
{
  if (path.empty())
  {
    FileOperationResult result;
    result.opResult = OperationResult::NoSuchFileOrDir;
    return result;
  }
  
  std::filesystem::path filePath(path);
  std::filesystem::path fileName = filePath.filename();
  std::filesystem::path dirInSanctum = filePath.parent_path();

  if (m_contentsTable.IsFileExist(dirInSanctum, fileName))
  {
    return GetFile(dirInSanctum, fileName);
  }
  /*
  else if (m_contentTable.ExistDir(path)) 
  {
    return GetDir(path);
  }

  return GetFileByName(fileName);
  */

  FileOperationResult result;
  result.opResult = OperationResult::NoSuchFileOrDir;
  return result;
}


//----------------------------------------------------------
/*
  Получить файл из хранилища
*/
//---
FileOperationResult DefaultCore::GetFile(const std::filesystem::path & dirInSanctum, const std::filesystem::path & fileName) const
{
  FileOperationResult result;
  std::filesystem::path finalPath = m_workDir / dirInSanctum / fileName;

  if (std::filesystem::exists(finalPath))
  {
    result.opResult = OperationResult::FileAlreadyExist;
    return result;
  }

  std::optional<FileDescription> actualFileDesc = m_contentsTable.GetActual(dirInSanctum, fileName);

  if (!actualFileDesc)
  {
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

  if (fileInSanctum.ReadFrom(sanctumFile, FileInsideSanctum::FileReadMode::Full, *m_encrypter))
  {
    if (fileInSanctum.SaveTo(m_workDir))
    {
      result.opResult = OperationResult::Ok;
    }
    else
    {
      result.opResult = OperationResult::FileProcessFail;
    }
  }
  else
  {
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
  result.opResult = OperationResult::Ok;
  std::filesystem::path putPath(path);

  if (putPath.empty()) // разместить рабочий файл
  {

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
  else  
  {
    if (putPath.has_filename()) // относительный путь к файлу в рабочей директории
    {
    }
    else // относительный путь к директории в рабочей директории
    {
    }
  }
 
  return result;
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
    int fileVersion = m_contentsTable.GetFileNextVersion(dirInSanctum.wstring(), putPath.filename().wstring());
    putFile.SetVersion(fileVersion);
    std::vector<FileInsideSanctum> putFiles{putFile};
    result.opResult = PutFiles(putFiles, PutFileMethod::Append);
  }
  else 
  {
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
    result.opResult = OperationResult::NoSuchFileOrDir;
    return result;
  }

  result.opResult = OperationResult::Ok;
    
  try 
  {
    std::vector<FileInsideSanctum> filesInSanctum;
    std::filesystem::path dirInSanctum = GetDirInSanctum(putPath);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(putPath)) 
    {
      if (std::filesystem::is_regular_file(entry)) 
      {
        std::wstring insideDirPath = GetFileDirUpTo(entry.path().wstring(), putPath.filename().wstring());
        std::wstring dirInSanctumFullPath = dirInSanctum / insideDirPath;
        int version = m_contentsTable.GetFileNextVersion(dirInSanctumFullPath, entry.path().filename());
        filesInSanctum.emplace_back(entry.path().wstring(), dirInSanctumFullPath, version);
      }
    }

    result.opResult = PutFiles(filesInSanctum, PutFileMethod::Append);
  } 
  catch (const std::filesystem::filesystem_error& err) 
  {
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
    if (*pathInWorkDir.begin() == L"..")
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
std::vector<FileDescription> DefaultCore::GetFileDescriptions() const 
{
  return m_contentsTable.GetDescriptions();
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
  m_contentsTable.Update(GetRelevantPath(), *m_encrypter);
  
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
    m_contentsTable.Update(GetRelevantPath(), *m_encrypter);
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
OperationResult DefaultCore::PutFiles(std::vector<FileInsideSanctum> & filesInSanctum, PutFileMethod method)
{
  std::unique_ptr<std::ofstream> fantomFile = GetFantomOutputStream(method);
  
  if (!fantomFile)
  {
    return OperationResult::NoSanctum;
  }

  OperationResult result = OperationResult::Ok;
  
  for (auto && nextFile : filesInSanctum)
  {
    if (nextFile.WriteTo(*fantomFile, *m_encrypter))
    {
      FileDescription newDesc;
      newDesc.version = nextFile.GetVersion();
      newDesc.name = nextFile.GetName();
      newDesc.dirName = nextFile.GetDirName();
      newDesc.offset = nextFile.GetOffset();
      m_contentsTable.AddFile(newDesc);
    }
    else
    {
      result = OperationResult::FileProcessFail;
      break;
    }
  }

  if (fantomFile->fail())
  {
    result = OperationResult::FileProcessFail;
  }

  fantomFile->close();
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
    return OperationResult::Ok; // нечего коммитить
  }

  if (std::filesystem::exists(m_sanctumPath))
  {
    std::filesystem::remove(m_sanctumPath);
  }
   
  std::filesystem::rename(GetRelevantPath(), m_sanctumPath);
  return OperationResult::Ok;
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
      std::filesystem::copy(m_sanctumPath, GetFantomPath());
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
  Создать ядро хранилища
*/
//---
std::unique_ptr<IfSanctumCore> CreateSanctumCore()
{
  return std::make_unique<DefaultCore>();
}

}