#include "pch.h"
#include "WindowsDialogs.h"

#include <algorithm>
#include <iterator>
#include <ShObjIdl.h>
#include <string>
#include <Windows.h>


const std::string OpenFileDialog::GetFileName() const
{
	//  FORMAT STRING FOR EXECUTABLE NAME
	std::string fileName;
	const size_t slash = m_fullPath.find_last_of("/\\");
	fileName = m_fullPath.substr(slash + 1);
	return fileName;
}

bool OpenFileDialog::Show()
{
	//  CREATE FILE OBJECT INSTANCE
	HRESULT f_SysHr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	if (FAILED(f_SysHr))
	{
		return FALSE;
	}

	// CREATE FileOpenDialog OBJECT
	IFileOpenDialog* f_FileSystem;
	f_SysHr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_IFileOpenDialog, reinterpret_cast<void**>(&f_FileSystem));
	if (FAILED(f_SysHr))
	{
		CoUninitialize();
		return FALSE;
	}

	const COMDLG_FILTERSPEC k_dialogFilter[3] = { {L"Saved AI Agents", L"*.bin;*.ai"}, {L"All Files",L"*.*"} };

	f_FileSystem->SetFileTypes(2, k_dialogFilter);
	f_FileSystem->SetTitle(L"Select AI Agents");

	//  SHOW OPEN FILE DIALOG WINDOW
	f_SysHr = f_FileSystem->Show(NULL);
	if (FAILED(f_SysHr))
	{
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	//  RETRIEVE FILE NAME FROM THE SELECTED ITEM
	IShellItem* f_Files;
	f_SysHr = f_FileSystem->GetResult(&f_Files);
	if (FAILED(f_SysHr))
	{
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	//  STORE AND CONVERT THE FILE NAME
	PWSTR f_Path;
	f_SysHr = f_Files->GetDisplayName(SIGDN_FILESYSPATH, &f_Path);
	if (FAILED(f_SysHr))
	{
		f_Files->Release();
		f_FileSystem->Release();
		CoUninitialize();
		return FALSE;
	}

	//  FORMAT AND STORE THE FILE PATH
	std::wstring path(f_Path);
	m_fullPath = "";
	// TODO: This conversion from std::wstring to std::string is a blind truncation of values
	//       and will fail for anything not part of standard ascii
	std::transform(path.begin(), path.end(), std::back_inserter(m_fullPath), [](wchar_t c) {
		return static_cast<char>(c);
		});

	//  SUCCESS, CLEAN UP
	CoTaskMemFree(f_Path);
	f_Files->Release();
	f_FileSystem->Release();
	CoUninitialize();
	return TRUE;
}
