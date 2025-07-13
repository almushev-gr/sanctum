#include "DefaultEncrypter.h"
#include "IfEncrypter.h"
#include <algorithm>


namespace sanctum::encrypter
{

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
  Получить политику работы с ключом
  В умолчательном шифраторе используется слабая защита
  внешним ключом-паролем на уровне ядра
*/
//---
KeyPolicy DefaultEncrypter::GetKeyPolicy() const
{
  return KeyPolicy::KeyForCall;
}


//----------------------------------------------------------
/*
  Проверить допустим ключ для шифрации или нет
  Так как в умолчательном шифраторе ключ не используется,
  то подходит любой ключ
*/
//---
bool DefaultEncrypter::IsKeyValid(const std::string & key) const 
{
  return true;
}


//----------------------------------------------------------
/*
  Создать шифратор
*/
//---
std::unique_ptr<IfEncrypter> Create()
{
  return std::make_unique<DefaultEncrypter>();
}

}