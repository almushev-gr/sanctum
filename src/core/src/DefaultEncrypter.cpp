#include "DefaultEncrypter.h"
#include <algorithm>


namespace sanctum::encrypter
{

//----------------------------------------------------------
/*
 
*/
//---
DefaultEncrypter::DefaultEncrypter()
  : m_key("defPass")
{
}


//----------------------------------------------------------
/*
  Зашифровать набор байтов
*/
//---
std::vector<char> DefaultEncrypter::Encrypt(const std::vector<char> & bytes)
{
  std::vector<char> encBytes(bytes);
  std::reverse(encBytes.begin(), encBytes.end());
  return encBytes;
}


//----------------------------------------------------------
/*
  Расшифровать набор байтов
*/
//---
std::vector<char> DefaultEncrypter::Decrypt(const std::vector<char> & bytes)
{
  std::vector<char> decBytes(bytes);
  std::reverse(decBytes.begin(), decBytes.end());
  return decBytes;
}


//----------------------------------------------------------
/*
  Зашифровать строку
*/
//---
std::string DefaultEncrypter::Encrypt(const std::string & str)
{
  std::vector<char> strAsVector(str.begin(), str.end());
  std::vector<char> encBytes = Encrypt(strAsVector);
  return std::string(encBytes.begin(), encBytes.end());
} 


//----------------------------------------------------------
/*
  Расшифровать строку
*/
//---
std::string DefaultEncrypter::Decrypt(const std::string & str)
{
  std::vector<char> strAsVector(str.begin(), str.end());
  std::vector<char> decBytes = Decrypt(strAsVector);
  return std::string(decBytes.begin(), decBytes.end());
}


//----------------------------------------------------------
/*
  Задать ключ шифрации
*/
//---
void DefaultEncrypter::SetKey(const std::string & key)
{
  m_key = key;
}

}