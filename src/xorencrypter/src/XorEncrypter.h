#pragma once
#include "IfEncrypter.h"


namespace sanctum::encrypter
{

//----------------------------------------------------------
/*
  Шифровальщик данных с xor-шифрацией
*/
//---
class XorEncrypter : public IfEncrypter
{
public:
  XorEncrypter() = default;
  virtual ~XorEncrypter() = default;

  virtual std::vector<char> Encrypt(const std::vector<char> & bytes, const std::string & key) override;
  virtual std::vector<char> Decrypt(const std::vector<char> & bytes, const std::string & key) override;
  virtual std::string Encrypt(const std::string & str, const std::string & key) override;
  virtual std::string Decrypt(const std::string & str, const std::string & key) override;
  virtual KeyPolicy GetKeyPolicy() const override;
  virtual std::string GetName() const override;
  
};

}