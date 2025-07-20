#pragma once
#include <IfSanctumCore.h>
#include <IfEncrypter.h>
#include <filesystem>
#include <optional>
#include <set>
#include <map>


namespace sanctum::core
{

//----------------------------------------------------------
/*
  Оглавление хранилища
  Предоставялет информацию о файловом составе хранилища,
  текущей версии каждого файла и его позиции в хранилище
  Позиция файла - это просто его порядковый номер 
  в хранилище, начиная с 0
*/
//--- 
class ContentsTable
{
private: 
  std::vector<FileDescription> m_fileDescs; ///< описания файлов
  std::map<std::wstring, std::set<std::wstring>> m_dirs; ///< директории файлов

public:
  ContentsTable() = default;
  ~ContentsTable() = default;

  const std::vector<FileDescription> & GetDescriptions() const { return m_fileDescs; }
  void Update(const std::filesystem::path & sanctumPath, sanctum::encrypter::IfEncrypter & encrypter, const std::string & key);
  void AddFile(const FileDescription & fileDesc);
  int GetFileNextVersion(const std::wstring & dirInSanctum, const std::wstring & fileName) const;
  bool IsFileExist(const std::filesystem::path & dirPath, const std::filesystem::path & fileName) const;
  std::optional<FileDescription> GetActual(const std::filesystem::path & dirPath, const std::filesystem::path & fileName) const;
  bool IsDirExist(const std::wstring & dirPath) const;
  std::vector<FileDescription> GetFilesInDir(const std::wstring & dirPath);
  std::set<std::filesystem::path> GetDirsContainsString(const std::wstring & str);
  std::set<std::filesystem::path> GetFilesContainsString(const std::wstring & str);

private:
  void AddDirByDescription(const FileDescription & desc);
  std::optional<int> GetActualVersion(const std::wstring & dirInSanctum, const std::wstring & fileName);
};

}