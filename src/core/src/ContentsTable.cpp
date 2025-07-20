#include "ContentsTable.h"
#include <FileInsideSanctum.h>
#include "IfSanctumCore.h"
#include <filesystem>
#include <fstream>
#include <algorithm>


namespace sanctum::core
{

//----------------------------------------------------------
/*
  Обновить оглавление хранилища
*/
//---
void ContentsTable::Update(const std::filesystem::path & sanctumPath, sanctum::encrypter::IfEncrypter & encrypter, const std::string & key)
{
   m_fileDescs.clear();
   m_dirs.clear();
  
  if (!std::filesystem::exists(sanctumPath))
  {
    return;
  }

  std::ifstream sanctumFile(sanctumPath.string(), std::ios::binary);

  if (!sanctumFile.is_open())
  {
    return;
  }

  sanctumFile.unsetf(std::ios::skipws);
  int i = 0;
  
  while (!sanctumFile.eof())
  {
    FileInsideSanctum nextFile;
        
    if (nextFile.ReadFrom(sanctumFile, FileInsideSanctum::FileReadMode::HeaderOnly, encrypter, key))
    {
      FileDescription nextDesc;
      nextDesc.offset = nextFile.GetOffset();
      nextDesc.name = nextFile.GetName();
      nextDesc.dirName = nextFile.GetDirName();
      nextDesc.version = nextFile.GetVersion();
      nextDesc.position = i;
      m_fileDescs.push_back(nextDesc);
      AddDirByDescription(nextDesc);
      i++;
    }
    else 
    {
      break;
    }
  }
  
  sanctumFile.close();
}


//----------------------------------------------------------
/*
  Добавить новое описание в конец оглавления
*/
//---
void ContentsTable::AddFile(const FileDescription & fileDesc)
{
  m_fileDescs.push_back(fileDesc);
  m_fileDescs.back().position = static_cast<int>(m_fileDescs.size()) - 1;
  AddDirByDescription(fileDesc);
}


//----------------------------------------------------------
/*
  Получить следующую версию файла
*/
//---
int ContentsTable::GetFileNextVersion(const std::wstring & dirInSanctum, const std::wstring & fileName) const
{
  int nextVersion = 0;

  for (auto && nextDesc : m_fileDescs)
  {
    if (nextDesc.name == fileName && nextDesc.dirName == dirInSanctum && nextDesc.version >= nextVersion)
    {
      nextVersion = nextDesc.version + 1;
    }
  }

  return nextVersion;
}


//----------------------------------------------------------
/*
  Существует ли файл в хранилище
*/
//---
bool ContentsTable::IsFileExist(const std::filesystem::path & dirPath, const std::filesystem::path & fileName) const
{
  auto it = std::find_if(m_fileDescs.begin(), m_fileDescs.end(), 
  [&dirPath, &fileName](const FileDescription & nextDesc){ return nextDesc.dirName == dirPath && nextDesc.name == fileName; });

  return it != m_fileDescs.end();
}


//----------------------------------------------------------
/*
  Найти файл наибольшей версии
*/
//---
std::optional<FileDescription> ContentsTable::GetActual(const std::filesystem::path & dirPath, const std::filesystem::path & fileName) const
{
  std::optional<FileDescription> result;

  for (auto && nextDesc : m_fileDescs)
  {
    if (nextDesc.dirName == dirPath && nextDesc.name == fileName)
    {
      if (!result || nextDesc.version > result->version)
      {
        result = nextDesc;
      }
    }
  }

  return result;
}


//----------------------------------------------------------
/*
  Добавить директорию по описанию файла
*/
//---
void ContentsTable::AddDirByDescription(const FileDescription & desc)
{
  std::filesystem::path nextDirPath = desc.dirName;

  while (true)
  {
    std::filesystem::path nextDirName = nextDirPath.filename();
    std::filesystem::path nextSubDirPath = nextDirPath.parent_path();

    m_dirs[nextDirName.wstring()].insert(nextSubDirPath.wstring());

    nextDirPath = nextSubDirPath;

    if (nextDirPath.empty())
    {
      break;
    }
  }
}


//----------------------------------------------------------
/*
  Проверить существует ли такая директория
  Проверка ведется по полному пути директории
*/
//---
bool ContentsTable::IsDirExist(const std::wstring & dirPath) const
{
  for (auto & [dirName, parentDirs] : m_dirs)
  {
    for (auto && nextParentDir : parentDirs)
    {
      std::filesystem::path parentDirPath = nextParentDir;
      std::filesystem::path fullDirPath = parentDirPath / std::filesystem::path{dirName};

      if (fullDirPath.wstring() == dirPath)
      {
        return true;
      }
    }
  }

  return false;
}


//----------------------------------------------------------
/*
  Получить файлы, содержащиеся в директории
*/
//---
std::vector<FileDescription> ContentsTable::GetFilesInDir(const std::wstring & dirPath)
{
  std::vector<FileDescription> result;

  std::copy_if(m_fileDescs.begin(), m_fileDescs.end(), std::back_inserter(result),
  [this, &dirPath](const FileDescription & nextDesc)
    {
      size_t posInLine = nextDesc.dirName.find(dirPath);

      if (posInLine == 0)
      {
        std::optional<int> actualVersion = GetActualVersion(nextDesc.dirName, nextDesc.name);
        return nextDesc.version == *actualVersion;
      }

      return false;
    });

  return result;
}


//----------------------------------------------------------
/*
  Получить актуальную версию файла
*/
//---
std::optional<int> ContentsTable::GetActualVersion(const std::wstring & dirInSanctum, const std::wstring & fileName)
{
  std::optional<int> actualVersion;

  for (auto && nextDesc : m_fileDescs)
  {
    if (nextDesc.name == fileName && nextDesc.dirName == dirInSanctum)
    {
      if (!actualVersion || nextDesc.version > *actualVersion)
      {
        actualVersion = nextDesc.version;
      }
    }
  }

  return actualVersion;
}


//----------------------------------------------------------
/*
  Получить директории, в имени которых есть строка
*/
//---
std::set<std::filesystem::path> ContentsTable::GetDirsContainsString(const std::wstring & str)
{
  std::set<std::filesystem::path> result;

  for (auto & [dirName, parentDirNames] : m_dirs)
  {
    if (dirName.find(str) != std::wstring::npos)
    {
      for (auto && nextParentName : parentDirNames)
      {
        std::filesystem::path dirPath = std::filesystem::path{nextParentName} / std::filesystem::path{dirName};
        result.insert(dirPath);
      }
    }
  }

  return result;
}


//----------------------------------------------------------
/*
  Получить файлы, в имени которых есть строка
*/
//---
std::set<std::filesystem::path> ContentsTable::GetFilesContainsString(const std::wstring & str)
{
  std::set<std::filesystem::path> filePaths;

  for (auto && nextDesc : m_fileDescs)
  {
    if (nextDesc.name.find(str) != std::wstring::npos)
    {
      std::filesystem::path filePath = std::filesystem::path{nextDesc.dirName} / std::filesystem::path{nextDesc.name};
      filePaths.insert(filePath);
    }
  }

  return filePaths;
}

}