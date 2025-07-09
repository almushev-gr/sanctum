#pragma once
#include <IfEncrypter.h>
#include <string>
#include <fstream>
#include <optional>
#include <filesystem>


namespace sanctum::core
{

/*-------------------------------------------

  Файл в контексте хранилища

---*/
class FileInsideSanctum
{
public:
  enum class FileReadMode
  {
    Full,
    HeaderOnly
  };

private:
  std::wstring m_fullPath; ///< полное имя файла на диске
  std::wstring m_dirInSanctum; ///< директория в контексте хранилища
  std::wstring m_name; ///< имя файла
  std::optional<size_t> m_contentSize; ///< размер содержательной части файла
  size_t m_offset{0}; ///< смещение внутри хранилища
  int m_version{0}; ///< версия файла
  std::vector<char> m_content; ///< содержимое файла

public:
  FileInsideSanctum() = default;
  FileInsideSanctum(const std::wstring & fullPath, const std::wstring & dirInSanctum, int version);
  ~FileInsideSanctum() = default;

  bool WriteTo(std::ofstream & output, sanctum::encrypter::IfEncrypter & encrypter);
  bool ReadFrom(std::ifstream & input, FileReadMode mode, sanctum::encrypter::IfEncrypter & encrypter);
  bool SaveTo(const std::filesystem::path & dirPath) const;
  const std::wstring & GetName() const { return m_name; }
  const std::wstring & GetDirName() const { return m_dirInSanctum; }
  std::optional<size_t> GetContentSize() const { return m_contentSize; }
  int GetVersion() const { return m_version; }
  size_t GetOffset() const { return m_offset; }
  void SetFullPath(const std::wstring & path);
  void SetDirInSanctum(const std::wstring & path) { m_dirInSanctum = path; }
  void SetVersion(int version) { m_version = version; }

private:
  bool WriteHeaderTo(std::ofstream & output, sanctum::encrypter::IfEncrypter & encrypter) const;
  bool ReadHeaderFrom(std::ifstream & input, sanctum::encrypter::IfEncrypter & encrypter);

};

}


