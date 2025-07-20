#pragma once
#include <vector>
#include <string>
#include <memory>


namespace sanctum::encrypter
{

// Политика использования ключа шифратором
enum class KeyPolicy
{
  NoKey, // не использовать ключ совсем
  KeyForEncryption, // использовать ключ для шифрования
  KeyForCall // использовать ключ для вызова функций
};


//----------------------------------------------------------
/*
  Интерфейс шифратора
  Шифрует набор байтов по определнному 
  алгоритму при участии ключ, либо без него
*/
//---
struct IfEncrypter
{
  virtual std::vector<char> Encrypt(const std::vector<char> & bytes, const std::string & key) = 0;
  virtual std::string Encrypt(const std::string & str, const std::string & key) = 0;
  virtual std::vector<char> Decrypt(const std::vector<char> & bytes, const std::string & key) = 0;
  virtual std::string Decrypt(const std::string & str, const std::string & key) = 0;
  virtual KeyPolicy GetKeyPolicy() const = 0;
  virtual std::string GetName() const = 0;
  virtual ~IfEncrypter() = default;
};


std::unique_ptr<IfEncrypter> Create();

}