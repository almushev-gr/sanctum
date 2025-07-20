#pragma once

#ifdef ENCRYPTOR_FUNC
#else
#define ENCRYPTOR_FUNC extern "C" __declspec(dllimport)
#endif

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
  virtual std::wstring GetName() const override;
  
};

ENCRYPTOR_FUNC IfEncrypter * GetEncrypter();

}


// ENCRYPTOR_FUNC sanctum::encrypter::IfEncrypter * GetEncrypter();