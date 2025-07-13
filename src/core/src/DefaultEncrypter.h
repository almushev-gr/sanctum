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

  virtual std::vector<char> Encrypt(const std::vector<char> & bytes) override;
  virtual std::vector<char> Decrypt(const std::vector<char> & bytes) override;
  virtual std::string Encrypt(const std::string & str) override;
  virtual std::string Decrypt(const std::string & str) override;
  virtual KeyPolicy GetKeyPolicy() const override;
  virtual bool IsKeyValid(const std::string & key) const override;
};

}