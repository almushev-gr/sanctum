#include "FileInsideSanctum.h"
#include <filesystem>
#include <codecvt>


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
      nextByte++;
    }

    return true;
  }


  //----------------------------------------------------------
  /*
    Записать строку в поток
  */
  //---
  void WriteStringToStream(std::ofstream & output, const std::string & str)
  {
    for (int i = 0; i < str.size(); ++i)
    {
      output << str[i];
    }
  }


  //----------------------------------------------------------
  /*
    Прочитать строку из потока
  */
  //---
  std::string ReadStringFromStream(std::ifstream & input, size_t strLength)
  {
    std::string result;

    for (int i = 0; i < strLength; ++i)
    {
      char nextChar;
      input >> nextChar;
      result.push_back(nextChar);
    }

    return result;
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
bool FileInsideSanctum::ReadFrom(std::ifstream & input, FileReadMode mode, sanctum::encrypter::IfEncrypter & encrypter)
{
  m_offset = input.tellg();

  if (!ReadHeaderFrom(input, encrypter))
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
    m_content = encrypter.Decrypt(m_content);
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
  std::filesystem::create_directories(dirPath / m_dirInSanctum);
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
bool FileInsideSanctum::ReadHeaderFrom(std::ifstream & input, sanctum::encrypter::IfEncrypter & encrypter)
{
  input.read(reinterpret_cast<char *>(&m_version), sizeof(int));

  if (input.fail())
  {
    return false;
  }

  size_t dirNameLength;
  input.read(reinterpret_cast<char *>(&dirNameLength), sizeof(size_t));

  std::string encDirName = ReadStringFromStream(input, dirNameLength);
  std::string decDirName = encrypter.Decrypt(encDirName);
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  m_dirInSanctum = converter.from_bytes(decDirName);

  size_t fileNameLength;
  input.read(reinterpret_cast<char *>(&fileNameLength), sizeof(size_t));

  std::string encFileName = ReadStringFromStream(input, fileNameLength);
  std::string decFileName = encrypter.Decrypt(encFileName);
  m_name = converter.from_bytes(decFileName);

  size_t fileSize;
  input.read(reinterpret_cast<char *>(&fileSize), sizeof(size_t));
  m_contentSize = fileSize;

  return true;
}


//----------------------------------------------------------
/*
  Записать файл в поток c шифрованием данных
*/
//---
bool FileInsideSanctum::WriteTo(std::ofstream & output, sanctum::encrypter::IfEncrypter & encrypter)
{
  std::streamoff offset = static_cast<std::streamoff>(output.tellp());
  m_offset = offset;

  if (!WriteHeaderTo(output, encrypter))
  {
    return false;
  }

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

  if (!file.fail())
  {
    file.close();

    std::vector<char> encFileBytes = encrypter.Encrypt(fileBytes);
    WriteDigitToStream<size_t>(output, encFileBytes.size());

    for (auto && nextByte : encFileBytes)
    {
      output << nextByte;
    }

    return true;
  }

  file.close();
  return false;
}


//----------------------------------------------------------
/*
  Записать заголовок файла в поток
*/
//---
bool FileInsideSanctum::WriteHeaderTo(std::ofstream & output, sanctum::encrypter::IfEncrypter & encrypter) const
{
  WriteDigitToStream<int>(output, m_version);
  
  std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
  std::string encDirName = encrypter.Encrypt(converter.to_bytes(m_dirInSanctum));

  WriteDigitToStream<size_t>(output, encDirName.size());
  WriteStringToStream(output, encDirName);

  std::string encFileName = encrypter.Encrypt(converter.to_bytes(m_name));

  WriteDigitToStream<size_t>(output, encFileName.size());
  WriteStringToStream(output, encFileName);

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

}