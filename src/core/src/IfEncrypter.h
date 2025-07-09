#pragma once
#include <vector>
#include <string>


namespace sanctum::encrypter
{

/*-------------------------------------------

  Интерфейс шифратора
  Шифрует набор байтов по определнному 
  алгоритму при участии ключ, либо без него

---*/
struct IfEncrypter
{
  virtual std::vector<char> Encrypt(const std::vector<char> & bytes) = 0;
  virtual std::string Encrypt(const std::string & str) = 0;
  virtual std::vector<char> Decrypt(const std::vector<char> & bytes) = 0;
  virtual std::string Decrypt(const std::string & str) = 0;
  virtual void SetKey(const std::string & key) = 0;
  //virtual bool IsKeyValid(const std::string & key) = 0;
  virtual ~IfEncrypter() = default;
};

// CreateEncrypter

}