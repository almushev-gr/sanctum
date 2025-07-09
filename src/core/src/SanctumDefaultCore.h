#pragma once
#include "IfEncrypter.h"
#include <IfSanctumCore.h>
#include <FileInsideSanctum.h>
#include <ContentsTable.h>
#include <vector>
#include <memory>
#include <fstream>
#include <filesystem>


namespace sanctum::core
{

//----------------------------------------------------------
/*
  Ядро хранилища
  Отвечает за размещение файлов в хранилище,
  за извлечение файлов из хранилища.
  При помещении файла в хранилище применяется шифрация.
  Алгоритм шифрации может быть изменен через выбор нужного
  шифратора (IfEncrypter)
*/
//---
class DefaultCore : public IfSanctumCore
{
private:
  std::filesystem::path m_workDir; ///< рабочая директория, куда извлекаются файлы
  std::filesystem::path m_workFile; ///< полный путь к рабочему файлу (может быть не задано)
  std::filesystem::path m_sanctumDir;  ///< директория с файлом хранилища (если не задана, то текущая)
  std::filesystem::path m_sanctumName; ///< имя файла хранилища (обязательно должно быть задано)
  std::filesystem::path m_sanctumPath; ///< путь к хранилищу (с закоммиченными изменениями)
  std::unique_ptr<sanctum::encrypter::IfEncrypter> m_encrypter; ///< шифровальщик данных (всегда есть)
  ContentsTable m_contentsTable; ///< оглавление хранилища

  // способ размещения файла в хранилище
  enum class PutFileMethod
  {
    ClearOthers, // с предварительной очисткой хранилища
    Append // добавить к остальным файлам в хранилище
  };

public:
  DefaultCore();
  virtual ~DefaultCore() = default;

  virtual OperationResult SetWorkDir(const std::wstring & dirFullPath) override;
  virtual OperationResult SetWorkFile(const std::wstring & fileFullPath) override;
  virtual OperationResult SetSanctumDir(const std::wstring & dirFullPath) override;
  virtual std::wstring GetSanctumDir() const override; 
  virtual OperationResult SetSanctumName(const std::wstring & name) override;
  virtual FileOperationResult Put(const std::wstring & path) override;
  virtual FileOperationResult Get(const std::wstring & path) override;
  virtual OperationResult Commit() override;
  virtual std::vector<FileDescription> GetFileDescriptions() const override;

private:
  std::wstring GetFileDirUpTo(const std::wstring & fileName, const std::wstring & dirName);
  std::filesystem::path GetFantomPath() const;
  std::filesystem::path GetRelevantPath() const;
  std::unique_ptr<std::ofstream> GetFantomOutputStream(PutFileMethod method);
  OperationResult PutFiles(std::vector<FileInsideSanctum> & fileDescs, PutFileMethod method);
  FileOperationResult PutFileByAbsPath(const std::filesystem::path & putPath);
  FileOperationResult PutDirByAbsPath(const std::filesystem::path & putPath);
  FileOperationResult GetFile(const std::filesystem::path & dirInSanctum, const std::filesystem::path & fileName) const;
  std::filesystem::path GetDirInSanctum(const std::filesystem::path & absPath) const;

};

}