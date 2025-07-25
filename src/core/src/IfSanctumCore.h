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
  NoWorkFile, // рабочий файл не задан
  NoSuchFileOrDir,
  FileProcessFail,  // ошибка при работе с файлом
  FileAlreadyExist,
  AmbiguousInput, // неоднозначные входные данные
  InvalidKey, // неверный ключ для шифрации
  KeyRequired, // требуется ключ для шифрации
  WrongEncrypter, // не тот шифратор
  WrongKeyOrEncrypter, // либо не тот шифратор, либо не тот ключ
  UnknownError,
  RemoveFileError,
  RenameFileError,
  OutsideEncrypterAlreadyLoaded, 
  OutsideEncrypterNotLoaded,
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
  std::vector<std::wstring> ambiguousFiles; ///< неоднозначные файлы для уточнения
  std::vector<std::wstring> problemFiles; ///< файлы, при обработке которых возникли проблемы
};


// Результат операции на содержанием хранилища
struct ContentsOperationResult
{
  OperationResult opResult; ///< результат операции
  std::vector<FileDescription> descs; ///< описания файлов
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
  virtual bool SaveConfig() const = 0;
  virtual void SetOperationKey(const std::string & key) = 0;
  virtual OperationResult LoadEncrypter(const std::wstring & encPath) = 0;
  virtual OperationResult UnloadEncrypter() = 0;
  virtual std::wstring GetEncrypterName() const = 0;
  virtual ContentsOperationResult GetFileDescriptions() = 0;
  virtual void ClearContents() = 0;
  virtual OperationResult Commit() = 0;
  virtual ~IfSanctumCore() = default;
};


std::unique_ptr<IfSanctumCore> CreateSanctumCore();

}
