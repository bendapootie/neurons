#pragma once

#include <map>
#include <string>
#include <vector>

class AiControllerData;

typedef std::vector<AiControllerData*> AiControllerList;

class FileToControllerMap
{
public:
	// 4-byte string to identify file type
	static const char* GetFileMagicString();
	// Major versions are fundamental feature breaks where data migration could not save all data
	static int GetFileMajorVersion();
	// Minor versions are not directly backward compatible, but data migration could be possible
	static int GetFileMinorVersion();
	// Newer revisions can read data from older revisions
	static int GetFileRevisionNumber();

	std::vector<std::string> GetAllFiles() const;
	const AiControllerList* GetControllerList(const std::string& filename) const;

	// Opens the given file and loads all the AiControllerData stored in it.
	// If an entry for the file already exists, it will be replaced
	void LoadFromFile(const std::string& filename);

private:
	// Maps from file name to file data
	std::map<std::string, AiControllerList> m_fileToControllerList;
};
