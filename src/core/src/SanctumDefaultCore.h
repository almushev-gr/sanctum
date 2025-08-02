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
  sanctum::encrypter::IfEncrypter * m_encrypter; ///< шифровальщик данных (всегда есть)
  std::unique_ptr<ContentsTable> m_contentsTable; ///< оглавление хранилища
  mutable std::string m_operationKey; ///< ключ шифрования для одной операции
  std::string m_permanentKey; ///< постоянный ключ шифрования 
  mutable size_t m_coreKeyHash; ///< хеш используемого ключа уровня ядра (для коммитов и шифраторов без ключа)
  std::filesystem::path m_outsideEncrypterPath; ///< путь загруженной внешней библиотеки шифратора

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
  virtual std::wstring GetWorkDir() const override; 
  virtual OperationResult SetSanctumDir(const std::wstring & dirFullPath) override;
  virtual std::wstring GetSanctumDir() const override; 
  virtual OperationResult SetSanctumName(const std::wstring & name) override;
  virtual OperationResult RenameSanctum(const std::wstring & newName) override;
  virtual std::wstring GetSanctumName() const override;
  virtual FileOperationResult Put(const std::wstring & path) override;
  virtual FileOperationResult Get(const std::wstring & path) override;
  virtual OperationResult Commit() override;
  virtual ContentsOperationResult GetFileDescriptions() override;
  virtual ContentsOperationResult GetCommitFileDescriptions() override;
  virtual void ClearContents() override;
  virtual bool SaveConfig() const override;
  virtual void SetOperationKey(const std::string & key) override;
  virtual OperationResult LoadEncrypter(const std::wstring & encPath) override;
  virtual OperationResult UnloadEncrypter() override;
  virtual std::wstring GetEncrypterName() const override;
  virtual KeyPolicy GetKeyPolicy() const override;
  virtual OperationResult ChangeCoreKey(const std::string & currentKey, const std::string & newKey) override;
  virtual bool IsPermanentKeyDefined() const override;
  virtual bool IsCoreKeyValid(const std::string & key) const override;
  virtual void DropCoreKey() override;
  virtual OperationResult IsEncKeyValid(const std::string & key) const override;

private:
  std::wstring GetFileDirUpTo(const std::wstring & fileName, const std::wstring & dirName);
  std::filesystem::path GetFantomPath() const;
  std::filesystem::path GetRelevantPath() const;
  std::unique_ptr<std::ofstream> GetFantomOutputStream(PutFileMethod method);
  FileOperationResult PutFiles(std::vector<FileInsideSanctum> & fileDescs, PutFileMethod method);
  FileOperationResult PutFileByAbsPath(const std::filesystem::path & putPath);
  FileOperationResult PutDirByAbsPath(const std::filesystem::path & putPath);
  FileOperationResult GetFile(const std::filesystem::path & dirInSanctum, const std::filesystem::path & fileName);
  std::filesystem::path GetDirInSanctum(const std::filesystem::path & absPath) const;
  std::vector<std::filesystem::path> FindFilesInWorkDir(const std::filesystem::path & fileName);
  void LoadConfig();
  OperationResult CheckKey() const;
  OperationResult CheckDecryption() const;
  std::string GetKey() const;
  OperationResult RemoveFromDisk(const std::vector<std::wstring> & paths);
  FileOperationResult GetDir(const std::wstring & dirPath);
  ContentsTable & GetContentsTable();

};

}