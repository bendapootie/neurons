#pragma once

#include <string>

class OpenFileDialog
{
public:
	bool Show();

	// Returns full path of chosen file
	const std::string& GetFullPath() const { return m_fullPath; }

	// Returns the filename of the selected file without its path
	const std::string GetFileName() const;

private:
	std::string m_fullPath;
};
