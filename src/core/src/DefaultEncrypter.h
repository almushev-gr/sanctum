#pragma once
#include <IfEncrypter.h>


namespace sanctum::encrypter
{

//----------------------------------------------------------
/*
  Шифровальщик данных по-умолчанию
  Используется, если не задан другой шифровальщик
*/
//---
class DefaultEncrypter : public IfEncrypter
{
public:
  DefaultEncrypter() = default;
  virtual ~DefaultEncrypter() = default;

  virtual std::vector<char> Encrypt(const std::vector<char> & bytes, const std::string & key) override;
  virtual std::vector<char> Decrypt(const std::vector<char> & bytes, const std::string & key) override;
  virtual std::string Encrypt(const std::string & str, const std::string & key) override;
  virtual std::string Decrypt(const std::string & str, const std::string & key) override;
  virtual KeyPolicy GetKeyPolicy() const override;
  virtual std::wstring GetName() const override;
  
};


IfEncrypter * GetEncrypter();

}