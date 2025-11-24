#pragma once
#include <string>
#include <memory>
#include <vector>
#include <optional>


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
  AddFileToCommitError, // ошибка при включении файла в коммит
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
  bool isPurgeCandidate{false}; // является ли файл кандидатом на очистку
};

// цель для команды очистки
struct PurgeTarget
{
  std::wstring fileName; // имя очищаемого файла
  std::optional<int> version; // версия файла
  bool isAllVersions{false}; // полная очистка файла
};

// режим операции
enum class OperationMode
{
  Scouting, // разведка, сбор данных без изменений
  Action // боевое применение
};

// результат очистки
struct PurgeResult
{
  OperationResult opResult; ///< результат операции общего вида
  std::vector<FileDescription> purgedFiles; ///< очищенные файлы
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


// способ работы с ключами хранилища
enum class KeyPolicy
{
  CoreKey, // с помощью ключа уровня ядра
  EncrypterKey, // с помощью ключа шифрования
  NoKey // без ключа 
};


using ProgressHandler = void(*)(int, int);


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
  virtual OperationResult RenameSanctum(const std::wstring & newName) = 0;
  virtual std::wstring GetSanctumName() const = 0;
  virtual FileOperationResult Put(const std::wstring & path) = 0;
  virtual FileOperationResult Get(const std::wstring & path) = 0;
  virtual bool SaveConfig() const = 0;
  virtual OperationResult LoadEncrypter(const std::wstring & encPath) = 0;
  virtual OperationResult UnloadEncrypter() = 0;
  virtual std::wstring GetEncrypterName() const = 0;
  virtual ContentsOperationResult GetFileDescriptions() = 0;
  virtual ContentsOperationResult GetCommitFileDescriptions() = 0;
  virtual FileOperationResult CheckFiles() = 0;
  virtual void ClearContents() = 0;
  virtual OperationResult Commit() = 0;
  virtual KeyPolicy GetKeyPolicy() const = 0;
  virtual OperationResult ChangeCoreKey(const std::string & currentKey, const std::string & newKey) = 0;
  virtual bool IsCoreKeyValid(const std::string & key) const = 0;
  virtual void DropCoreKey() = 0;
  virtual void SetPermanentKey(const std::string & key) = 0;
  virtual OperationResult IsEncKeyValid(const std::string & key) const = 0;
  virtual bool IsPermanentKeyDefined() const = 0;
  virtual void SetOperationKey(const std::string & key) = 0;
  virtual std::string GetOperationKey() const = 0;
  virtual void SetProgressHandler(ProgressHandler handler) = 0;
  virtual PurgeResult MarkFilesAsPurged(OperationMode opMode, const PurgeTarget & target) = 0;
  virtual PurgeResult MarkFilesAsActive(OperationMode opMode, const PurgeTarget & target) = 0;
  virtual ~IfSanctumCore() = default;
};


std::unique_ptr<IfSanctumCore> CreateSanctumCore();

}
