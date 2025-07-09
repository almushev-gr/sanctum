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
void ContentsTable::Update(const std::filesystem::path & sanctumPath, sanctum::encrypter::IfEncrypter & encrypter)
{
   m_fileDescs.clear();
  
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
        
    if (nextFile.ReadFrom(sanctumFile, FileInsideSanctum::FileReadMode::HeaderOnly, encrypter))
    {
      FileDescription nextDesc;
      nextDesc.offset = nextFile.GetOffset();
      nextDesc.name = nextFile.GetName();
      nextDesc.dirName = nextFile.GetDirName();
      nextDesc.version = nextFile.GetVersion();
      nextDesc.position = i;
      m_fileDescs.push_back(nextDesc);
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

}