#include "pch.h"
#include "FileToControllerMap.h"


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
const char* FileToControllerMap::GetFileMagicString()
{
	return k_fileMagicString;
}

//static
int FileToControllerMap::GetFileMajorVersion()
{
	return k_fileMajorVersion;
}

// static
int FileToControllerMap::GetFileMinorVersion()
{
	return k_fileMinorVersion;
}

//static
int FileToControllerMap::GetFileRevisionNumber()
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
	buffer.ReadBytes(magicString, static_cast<int>(strlen(FileToControllerMap::GetFileMagicString())));
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





void FileToControllerMap::LoadFromFile(const std::string& filename)
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
