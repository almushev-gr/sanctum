#pragma once
#include <string>
#include <memory>
#include <vector>


namespace sanctum::core
{

// Результат операции
enum class OperationResult
{
  NoSanctum,
  NoSuchFileOrDir,
  FileProcessFail,  // ошибка при работе с файлом
  FileAlreadyExist,
  Ok
};

// Описание файла
struct FileDescription
{
  int version{0};
  int position{0}; // порядковый номер файла в хранилище
  std::wstring name;
  std::wstring dirName;
  size_t offset{0}; // смещение внутри хранилища
};


// Результат операции над файлами
struct FileOperationResult
{
  OperationResult opResult; ///< результат операции
  std::vector<FileDescription> ambiguousFiles; ///< неоднозначные файлы для уточнения
};


//----------------------------------------------------------
/*
  Интерфейс ядра хранилища
*/
//---
struct IfSanctumCore
{
  virtual OperationResult SetWorkDir(const std::wstring & dirFullPath) = 0;
  virtual std::wstring GetWorkDir() const = 0; 
  virtual OperationResult SetWorkFile(const std::wstring & fileFullPath) = 0;
  virtual OperationResult SetSanctumDir(const std::wstring & dirFullPath) = 0;
  virtual std::wstring GetSanctumDir() const = 0; 
  virtual OperationResult SetSanctumName(const std::wstring & name) = 0;
  virtual FileOperationResult Put(const std::wstring & path) = 0;
  virtual FileOperationResult Get(const std::wstring & path) = 0;
  //virtual SaveConfig
  //virtual SetEncryptor
  //vittual GetEncryptor
  virtual std::vector<FileDescription> GetFileDescriptions() const = 0;
  virtual OperationResult Commit() = 0;
  virtual ~IfSanctumCore() = default;
};


std::unique_ptr<IfSanctumCore> CreateSanctumCore();

}
