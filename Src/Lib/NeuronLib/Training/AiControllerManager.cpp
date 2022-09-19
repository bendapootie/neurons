#include "pch.h"
#include "AiControllerManager.h"


#include <algorithm>
#include <fstream>
#include "Training\AiControllerData.h"
#include "Util\BinaryBuffer.h"
#include "Util\Random.h"


const char* k_fileMagicString = "pcAI";
constexpr int k_fileMajorVersion = 0;
constexpr int k_fileMinorVersion = 1;
constexpr int k_fileRevisionNumber = 0;

//static
const char* AiControllerManager::GetFileMagicString()
{
	return k_fileMagicString;
}

//static
int AiControllerManager::GetFileMajorVersion()
{
	return k_fileMajorVersion;
}

// static
int AiControllerManager::GetFileMinorVersion()
{
	return k_fileMinorVersion;
}

//static
int AiControllerManager::GetFileRevisionNumber()
{
	return k_fileRevisionNumber;
}


static bool LoadControllersFromFile(AiControllerList& outControllers, const char* inputFileName)
{
	if (inputFileName == nullptr)
	{
		return false;
	}

	// Read entire file into a vector
	// TODO: Read directly into Buffer
	std::ifstream inFile(inputFileName, std::ios::binary);
	if (inFile.fail())
	{
		return false; // "No matching file found");
	}

	std::vector<char> rawFileBytes(std::istreambuf_iterator<char>(inFile), {});
	inFile.close();

	HeapBuffer buffer(static_cast<int>(rawFileBytes.size()));
	buffer.WriteBytes(&rawFileBytes[0], static_cast<int>(rawFileBytes.size()));

	buffer.Seek(0);

	char magicString[16] = { 0 };
	int fileMajorVersion = 0;
	int fileMinorVersion = 0;
	int fileRevisionNumber = 0;
	buffer.ReadBytes(magicString, static_cast<int>(strlen(AiControllerManager::GetFileMagicString())));
	buffer.ReadInt(fileMajorVersion);
	buffer.ReadInt(fileMinorVersion);
	buffer.ReadInt(fileRevisionNumber);

	int numControllers = 0;
	buffer.ReadInt(numControllers);
	outControllers.resize(numControllers);
	for (AiControllerData*& controller : outControllers)
	{
		if (controller == nullptr)
		{
			// TODO: Can this parameter avoid being passed?
			Random defaultRand;
			controller = new AiControllerData(defaultRand);
		}
		controller->Deserialize(buffer);
	}

	const bool success = buffer.GetErrorStatus() == BinaryBuffer::ErrorStatus::NoError;
	_ASSERT(success);

	return success;
}



std::vector<std::string> AiControllerManager::GetAllFiles() const
{
	std::vector<std::string> allFiles;

	for (auto it : m_fileToControllerList)
	{
		allFiles.push_back(it.first);
	}

	return allFiles;
}

const AiControllerList* AiControllerManager::GetControllerList(const std::string& filename) const
{
	const AiControllerList* returnList = nullptr;

	const auto it = m_fileToControllerList.find(filename);
	if (it != m_fileToControllerList.end())
	{
		returnList = &it->second;
	}

	return returnList;
}

void AiControllerManager::LoadFromFile(const std::string& filename)
{
	AiControllerList& controllerList = m_fileToControllerList[filename];

	// Clear any old entries for file
	for (auto data : controllerList)
	{
		delete data;
	}
	controllerList.clear();

	bool success = LoadControllersFromFile(controllerList, filename.c_str());
	// TODO: Handle errors more gracefully
	_ASSERT(success);
}
