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
  std::vector<char> encBytes(bytes);
  //std::reverse(encBytes.begin(), encBytes.end());
  return encBytes;
}


//----------------------------------------------------------
/*
  Расшифровать набор байтов
*/
//---
std::vector<char> XorEncrypter::Decrypt(const std::vector<char> & bytes, const std::string & key)
{
  std::vector<char> decBytes(bytes);
  //std::reverse(decBytes.begin(), decBytes.end());
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
std::string XorEncrypter::GetName() const
{
  return "xor";
}


//----------------------------------------------------------
/*
  Создать шифратор
*/
//---
std::unique_ptr<IfEncrypter> Create()
{
  return std::make_unique<XorEncrypter>();
}

}