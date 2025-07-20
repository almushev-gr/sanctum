#include <windows.h>
#define ENCRYPTOR_FUNC extern "C" __declspec(dllexport)

#include "XorEncrypter.h"


namespace sanctum::encrypter
{

//----------------------------------------------------------
/*
  Зашифровать набор байтов
*/
//---
std::vector<char> XorEncrypter::Encrypt(const std::vector<char> & bytes, const std::string & key)
{
  std::vector<char> encBytes(bytes.size());

  for (size_t i=0; i<bytes.size(); i++)
  {
    encBytes[i] = bytes[i] ^ key[i % key.length()];
  }
  
  return encBytes;
}


//----------------------------------------------------------
/*
  Расшифровать набор байтов
*/
//---
std::vector<char> XorEncrypter::Decrypt(const std::vector<char> & bytes, const std::string & key)
{
  std::vector<char> decBytes(bytes.size());

  for (size_t i=0; i<bytes.size(); i++)
  {
    decBytes[i] = bytes[i] ^ key[i % key.length()];
  }
    
  return decBytes;
}


//----------------------------------------------------------
/*
  Зашифровать строку
*/
//---
std::string XorEncrypter::Encrypt(const std::string & str, const std::string & key)
{
  std::vector<char> strAsVector(str.begin(), str.end());
  std::vector<char> encBytes = Encrypt(strAsVector, key);
  return std::string(encBytes.begin(), encBytes.end());
} 


//----------------------------------------------------------
/*
  Расшифровать строку
*/
//---
std::string XorEncrypter::Decrypt(const std::string & str, const std::string & key)
{
  std::vector<char> strAsVector(str.begin(), str.end());
  std::vector<char> decBytes = Decrypt(strAsVector, key);
  return std::string(decBytes.begin(), decBytes.end());
}


//----------------------------------------------------------
/*
  Получить политику работы с ключом
*/
//---
KeyPolicy XorEncrypter::GetKeyPolicy() const
{
  return KeyPolicy::KeyForEncryption;
}


//----------------------------------------------------------
/*
  Получить имя шифратора
*/
//---
std::wstring XorEncrypter::GetName() const
{
  return L"xor";
}


//----------------------------------------------------------
/*
  Получить шифратор
*/
//---
IfEncrypter * GetEncrypter()
{
  static XorEncrypter encrypter;
  return &encrypter;
}

}


