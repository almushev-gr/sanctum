#include "FileInsideSanctum.h"
#include <filesystem>
#include <codecvt>
#include <stdexcept>


namespace 
{
  //----------------------------------------------------------
  /*
    Записать любое число в поток
  */
  //---
  template <typename T>
  bool WriteDigitToStream(std::ofstream & output, T element)
  {
    const char * nextByte = reinterpret_cast<const char*>(&element);

    for (int i = 0; i < sizeof(element); ++i)
    {
      output << *nextByte;

      if (output.fail())
      {
        return false;
      }

      nextByte++;
    }

    return true;
  }


  //----------------------------------------------------------
  /*
    Записать строку в поток
  */
  //---
  bool WriteStringToStream(std::ofstream & output, const std::string & str)
  {
    for (int i = 0; i < str.size(); ++i)
    {
      output << str[i];

      if (output.fail())
      {
        return false;
      }
    }

    return true;
  }


  //----------------------------------------------------------
  /*
    Прочитать строку из потока
  */
  //---
  std::optional<std::string> ReadStringFromStream(std::ifstream & input, size_t strLength)
  {
    std::string result;

    for (int i = 0; i < strLength; ++i)
    {
      char nextChar;
      input >> nextChar;

      if (input.fail())
      {
        return std::nullopt;
      }

      result.push_back(nextChar);
    }

    return result;
  }


  //----------------------------------------------------------
  /*
    Прочитать и декодировать строку из потока
  */
  //---
  bool ReadDecStringFromStream(std::wstring & outStr, std::ifstream & input,  
    sanctum::encrypter::IfEncrypter & encrypter, const std::string & key)
  {
    size_t strLength;
    input.read(reinterpret_cast<char *>(&strLength), sizeof(size_t));

    if (input.fail())
    {
      return false;
    }

    std::optional<std::string> encStr = ReadStringFromStream(input, strLength);

    if (!encStr)
    {
      return false;
    }

    std::string decStr = encrypter.Decrypt(*encStr, key);
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

    try
    {
      outStr = converter.from_bytes(decStr);
    }
    catch (const std::range_error & ex)
    {
      outStr = std::wstring(decStr.size(), L'#');
    }

    return true;
  }


  //----------------------------------------------------------
  /*
    Зашифровать строку и поместить в поток
  */
  //---
  bool WriteEncStringToStream(const std::wstring & str, std::ofstream & output, 
    sanctum::encrypter::IfEncrypter & encrypter, const std::string & key)
  {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::string encString;
    
    try 
    {
      encString = encrypter.Encrypt(converter.to_bytes(str), key);
    }
    catch (const std::range_error & ex)
    {
      return false;
    }
      
    if (!WriteDigitToStream<size_t>(output, encString.size()))
    {
      return false;
    }

    if (!WriteStringToStream(output, encString))
    {
      return false;
    }

    return true;
  }

}


namespace sanctum::core
{

//----------------------------------------------------------
/*
  
*/
//---
FileInsideSanctum::FileInsideSanctum(const std::wstring & fullPath, const std::wstring & dirInSanctum, int version)
  : m_fullPath(fullPath)
  , m_dirInSanctum(dirInSanctum)
  , m_version(version)
{
  m_name = std::filesystem::path(fullPath).filename().wstring();
}


//----------------------------------------------------------
/*
  Прочитать файл из потока
*/
//---
bool FileInsideSanctum::ReadFrom(std::ifstream & input, FileReadMode mode, sanctum::encrypter::IfEncrypter & encrypter, const std::string & key)
{
  m_offset = input.tellg();

  if (!ReadHeaderFrom(input, encrypter, key))
  {
    return false;
  }

  if (!m_contentSize)
  {
    return false;
  }

  if (mode == FileReadMode::HeaderOnly)
  {
    if (*m_contentSize > 0)
    {
      input.seekg(*m_contentSize, std::ios::cur);
    }
  }
  else 
  {
    m_content.resize(*m_contentSize);
    input.read(m_content.data(), *m_contentSize);
    m_content = encrypter.Decrypt(m_content, key);
  }

  return true;
}


//----------------------------------------------------------
/*
  Сохранить файл в директорию
*/
//---
bool FileInsideSanctum::SaveTo(const std::filesystem::path & dirPath) const
{
  std::filesystem::path finalPath = dirPath / m_dirInSanctum / m_name;

  try 
  {
    std::filesystem::create_directories(dirPath / m_dirInSanctum);
  }
  catch (const std::filesystem::filesystem_error& ex)
  {
    return false;
  }

  std::ofstream output(finalPath.wstring().c_str(), std::ios::binary);
  
  if (output.is_open()) 
  {
    output.write(reinterpret_cast<const char*>(m_content.data()), m_content.size() * sizeof(char));
    output.close();
    return true;
  }

  return false;
}


//----------------------------------------------------------
/*
  Прочитать заголовок из потока
*/
//---
bool FileInsideSanctum::ReadHeaderFrom(std::ifstream & input, sanctum::encrypter::IfEncrypter & encrypter, const std::string & key)
{
  if (!ReadDecStringFromStream(m_encName, input, encrypter, key))
  {
    return false;
  }

  input.read(reinterpret_cast<char *>(&m_version), sizeof(int));

  if (input.fail())
  {
    return false;
  }

  if (!ReadDecStringFromStream(m_dirInSanctum, input, encrypter, key))
  {
    return false;
  }

  if (!ReadDecStringFromStream(m_name, input, encrypter, key))
  {
    return false;
  }

  char checkSum;
  input.read(reinterpret_cast<char *>(&checkSum), sizeof(char));
  m_checkSum = checkSum;

  if (input.fail())
  {
    return false;
  }

  bool isPurgeCandidate = false;
  input.read(reinterpret_cast<char *>(&isPurgeCandidate), sizeof(bool));
  
  if (input.fail())
  {
    return false;
  }

  m_isPurgeCandidate = isPurgeCandidate;
  
  size_t fileSize;
  input.read(reinterpret_cast<char *>(&fileSize), sizeof(size_t));

  if (input.fail())
  {
    return false;
  }

  m_contentSize = fileSize;
  return true;
}


namespace 
{

//----------------------------------------------------------
/*
  Расчитать контрольную сумму для набора байт
*/
//---
char GetXorCheckSum(const std::vector<char> & bytes)
{
  char result = 0;

  for (char nextChar : bytes)
  {
    result ^= nextChar;
  }

  return result;
}

}


//----------------------------------------------------------
/*
  Записать файл в поток c шифрованием данных
*/
//---
bool FileInsideSanctum::WriteTo(std::ofstream & output, sanctum::encrypter::IfEncrypter & encrypter, const std::string & key)
{
  m_offset = static_cast<std::streamoff>(output.tellp());
  std::ifstream file(m_fullPath.c_str(), std::ios::binary);

  if (!file.is_open())
  {
    return false;
  }

  file.unsetf(std::ios::skipws);
  file.seekg(0, std::ios::end);
  std::streampos fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> fileBytes(fileSize);
  file.read(fileBytes.data(), fileSize);
  m_checkSum = GetXorCheckSum(fileBytes);

  if (!file.fail())
  {
    file.close();

    std::vector<char> encFileBytes = encrypter.Encrypt(fileBytes, key);
    m_contentSize = encFileBytes.size();
    m_encName = encrypter.GetName();

    if (WriteHeaderTo(output, encrypter, key))
    {
      for (auto && nextByte : encFileBytes)
      {
        output << nextByte;
      }

      return true;
    }
  }

  file.close();
  return false;
}


//----------------------------------------------------------
/*
  Записать заголовок файла в поток
*/
//---
bool FileInsideSanctum::WriteHeaderTo(std::ofstream & output, sanctum::encrypter::IfEncrypter & encrypter, const std::string & key) const
{
  if (!m_contentSize || m_encName.empty() || !m_checkSum)
  {
    return false;
  }

  if (!WriteEncStringToStream(m_encName, output, encrypter, key))
  {
    return false;
  }

  if (!WriteDigitToStream<int>(output, m_version))
  {
    return false;
  }

  if (!WriteEncStringToStream(m_dirInSanctum, output, encrypter, key))
  {
    return false;
  }
 
  if (!WriteEncStringToStream(m_name, output, encrypter, key))
  {
    return false;
  }

  if (!WriteDigitToStream<char>(output, *m_checkSum))
  {
    return false;
  }

  if (!WriteDigitToStream<bool>(output, m_isPurgeCandidate))
  {
    return false;
  }
  
  if (!WriteDigitToStream<size_t>(output, *m_contentSize))
  {
    return false;
  }
 
  return true;
}


//----------------------------------------------------------
/*
  Задать полный путь к файлу
*/
//---
void FileInsideSanctum::SetFullPath(const std::wstring & path)
{
  m_fullPath = path;
  m_name = std::filesystem::path(path).filename().wstring();
}


//----------------------------------------------------------
/*
  Проверить контрольную сумму считанных байтов файла
  Нужно проверить совпадает ли она с той, что в шапке
*/
//---
bool FileInsideSanctum::IsValidCheckSum() const
{
  if (!m_checkSum)
  {
    return false;
  }

  return GetXorCheckSum(m_content) == *m_checkSum;
}

}