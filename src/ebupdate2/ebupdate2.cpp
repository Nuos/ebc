// ebupdate2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "windows.h"
#include <string>
#include <time.h>
#include "tlhelp32.h"
#include "shlwapi.h"
#include "Psapi.h"

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )	// ��������DOS����

bool ExecDosCmd(LPCSTR sAppName,LPSTR sCommandLine,std::string& sOutString,bool bShow=true)
{
	SECURITY_ATTRIBUTES sa;  
	HANDLE hRead,hWrite;  
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);  
	sa.lpSecurityDescriptor = NULL;  
	sa.bInheritHandle = TRUE;  
	if (!CreatePipe(&hRead,&hWrite,&sa,0))   
	{  
		return false;
	}

	STARTUPINFO si;  
	PROCESS_INFORMATION pi;   
	si.cb = sizeof(STARTUPINFO);  
	GetStartupInfo(&si);   
	si.hStdError = hWrite;            //�Ѵ������̵ı�׼��������ض��򵽹ܵ�����  
	si.hStdOutput = hWrite;           //�Ѵ������̵ı�׼����ض��򵽹ܵ�����  
	si.wShowWindow = bShow?SW_SHOW:SW_HIDE;  
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;  
	//�ؼ����裬CreateProcess�����������������MSDN  
	if (!CreateProcess(sAppName, sCommandLine,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))   
	{  
		CloseHandle(hWrite);  
		CloseHandle(hRead);  
		return false;  
	}  
	CloseHandle(hWrite);  
	char buffer[4096] = {0};          //��4K�Ŀռ����洢��������ݣ�ֻҪ������ʾ�ļ����ݣ�һ��������ǹ����ˡ�  
	DWORD bytesRead;   
	while (true)   
	{  
		memset(buffer,0,sizeof(buffer));
		if (ReadFile(hRead,buffer,4095,&bytesRead,NULL) == NULL)  
			break;
		sOutString.append(buffer);
		//sOutString = buffer;
		//buffer�о���ִ�еĽ�������Ա��浽�ı���Ҳ����ֱ�����  
		//AfxMessageBox(buffer);   //�����ǵ����Ի�����ʾ  
	}  
	CloseHandle(hRead);   
	return true;
} 
#include <direct.h>
#include <io.h>
int copyDir(const char * pSrc,const char *pDes, const char * pBackup=NULL)
{
	if (NULL == pSrc || NULL == pDes)	return -1;
	mkdir(pDes);
	if (pBackup!=NULL && strlen(pBackup)>0)
	{
		mkdir(pBackup);
	}
	char dir[MAX_PATH] = {0};
	char srcFileName[MAX_PATH] = {0};
	char desFileName[MAX_PATH] = {0};
	char backFileName[MAX_PATH] = {0};

	char *str = "\\*.*";
	strcpy(dir,pSrc);
	strcat(dir,str);

	//���Ȳ���dir�з���Ҫ����ļ�
	long hFile = 0;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			strcpy(srcFileName,pSrc);
			strcat(srcFileName,"\\");
			strcat(srcFileName,fileinfo.name);

			strcpy(desFileName,pDes);
			strcat(desFileName,"\\");
			strcat(desFileName,fileinfo.name);

			if (pBackup!=NULL && strlen(pBackup)>0)
			{
				strcpy(backFileName,pBackup);
				strcat(backFileName,"\\");
				strcat(backFileName,fileinfo.name);
			}

			//����ǲ���Ŀ¼
			//�������Ŀ¼,����д����ļ���������ļ�
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				if (strlen(backFileName)>0)
				{
					CopyFile(desFileName,backFileName,FALSE);
				}
				if (!CopyFile(srcFileName,desFileName,FALSE))
				{
					_findclose(hFile);
					return -1;
				}
				//copyFile(srcFileName,desFileName);
			}
			else//����Ŀ¼���ݹ����
			{
				if ( strcmp(fileinfo.name, "." ) != 0 && strcmp(fileinfo.name, ".." ) != 0 )
				{
					//if (!::PathFileExists(desFileName))
					//	::CreateDirectory(desFileName,NULL);

					if (copyDir(srcFileName,desFileName,backFileName)!=1)
					{
						_findclose(hFile);
						return -2;
					}
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
		return 1;
	}
	return -3;
}
int deleteDir(const char * pSrc)
{
	if (NULL == pSrc)	return -1;
	char dir[MAX_PATH] = {0};
	char srcFileName[MAX_PATH] = {0};
	char desFileName[MAX_PATH] = {0};

	char *str = "\\*.*";
	strcpy(dir,pSrc);
	strcat(dir,str);

	//���Ȳ���dir�з���Ҫ����ļ�
	long hFile = 0;
	_finddata_t fileinfo;
	if ((hFile = _findfirst(dir,&fileinfo)) != -1)
	{
		do
		{
			strcpy(srcFileName,pSrc);
			strcat(srcFileName,"\\");
			strcat(srcFileName,fileinfo.name);

			//����ǲ���Ŀ¼
			//�������Ŀ¼,����д����ļ���������ļ�
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				DeleteFile(srcFileName);
			}
			else//����Ŀ¼���ݹ����
			{
				if ( strcmp(fileinfo.name, "." ) != 0 && strcmp(fileinfo.name, ".." ) != 0 )
				{
					deleteDir(srcFileName);
				}
			}
		} while (_findnext(hFile,&fileinfo) == 0);
		_findclose(hFile);
		RemoveDirectory(pSrc);
		return 1;
	}
	return -3;
}

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef _DEBUG
	const std::string strRarFile = "D:\\tn\\entboost\\ebc\\version\\pc-1.21.0.454.rar";
	const std::string strExeName = "D:\\tn\\entboost\\ebc\\version\\ebc.exe";
#else
	if (argc <= 2) return -1;
	const std::string strRarFile = argv[1];
	const std::string strExeName = argv[2];
#endif
	if (strRarFile.empty() || strExeName.empty()) return FALSE;
	if (!::PathFileExists(strRarFile.c_str()) || !::PathFileExists(strExeName.c_str())) return FALSE;

	int nCount = 0;
	bool bKillApp = false;
	while (true)
	{
		TCHAR szPath[MAX_PATH];
		Sleep(1000);
		HANDLE hProcessSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);  
		if (hProcessSnap==NULL) break;
		PROCESSENTRY32 pe32;  
		memset(&pe32,0,sizeof(pe32));
		pe32.dwSize=sizeof(PROCESSENTRY32);  
		bool bExistApp = false;
		if(::Process32First(hProcessSnap,&pe32))  
		{  
			do  
			{
				std::string::size_type find = strExeName.find(pe32.szExeFile);
				if (find != std::string::npos)
				{
					std::string sExePath(strExeName.substr(0,find));
					find = sExePath.find(":\\");
					if (find!=std::string::npos)
						sExePath = sExePath.substr(find+2);
					if (bKillApp)
					{
						HANDLE hProcess = OpenProcess(SYNCHRONIZE|PROCESS_TERMINATE|PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
						//HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);
						if(NULL != hProcess)
						{
							DWORD nSize = MAX_PATH;
							memset(szPath,0,nSize);
							GetProcessImageFileName(hProcess, szPath, MAX_PATH);
							std::string sFindPath(szPath);
							if (sFindPath.empty() || sFindPath.find(sExePath)!=std::string::npos)
								TerminateProcess(hProcess, 0);
							else
								CloseHandle(hProcess);
						}
						//break;
					}else
					{
						HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pe32.th32ProcessID);
						if(NULL != hProcess)
						{
							DWORD nSize = MAX_PATH;
							memset(szPath,0,nSize);
							GetProcessImageFileName(hProcess, szPath, MAX_PATH);
							std::string sFindPath(szPath);
							if (sFindPath.empty() || sFindPath.find(sExePath)!=std::string::npos)
							{
								CloseHandle(hProcess);
								bExistApp = true;
								break;
							}else
							{
								CloseHandle(hProcess);
							}
						}else
						{
							bExistApp = true;
							break;
						}
					}
				}
			}while(::Process32Next(hProcessSnap,&pe32));   
		}
		CloseHandle(hProcessSnap);

		if (bExistApp)
		{
			if (++nCount < 5) continue;	// �ȴ�5������
			// �г���û���˳�
			TCHAR lpszText[250];
			sprintf(lpszText,_T("���ڸ��Ƹ����ļ������˳�IM����%s����������£�ȷ��ǿ�Ƹ��£�ȡ���˳����£�"),pe32.szExeFile);
			if (MessageBox(NULL,lpszText,_T("�Զ����³���"),MB_OKCANCEL|MB_ICONQUESTION)==IDOK)
			{
				bKillApp = true;
			}else
			{
				//continue;
				return 0;
			}
		}else
		{
			break;
		}
	}

#ifdef _DEBUG
	std::string sAppPath(strExeName);
	sAppPath = sAppPath.substr(0,sAppPath.rfind('\\'));
	const std::string strToPath = strExeName.substr(0,strExeName.rfind('\\'));
#else
	TCHAR lpszModuleFileName[MAX_PATH];
	GetModuleFileName( NULL, lpszModuleFileName, MAX_PATH);
	std::string sAppPath(lpszModuleFileName);
	sAppPath = sAppPath.substr(0,sAppPath.rfind('\\'));
	// ��ѹ����һ��Ŀ¼;
	const std::string strToPath = sAppPath.substr(0,sAppPath.rfind('\\'));
#endif
	char lpszBuffer[260];
	sprintf(lpszBuffer,"%s\\update_temp",strToPath.c_str());
	const std::string strToPathTemp(lpszBuffer);
	if (::PathFileExists(strToPathTemp.c_str()))
		::deleteDir(strToPathTemp.c_str());
	mkdir(strToPathTemp.c_str());

	char lpszUpdateBackupDir[260];
	sprintf(lpszUpdateBackupDir,"%s\\update_bakcup",strToPath.c_str());
	if (::PathFileExists(lpszUpdateBackupDir))
		::deleteDir(lpszUpdateBackupDir);
	mkdir(lpszUpdateBackupDir);

	std::string strRarToFiles = "x -o+ \""+strRarFile+"\" ";
	strRarToFiles +=  "\"";
	strRarToFiles += strToPathTemp;
	strRarToFiles += "\"";

	bool bOK = false;
	char lpszCommand[1024];
	sprintf(lpszCommand,"Rar.exe %s",strRarToFiles.c_str());
	for (int i=0; i<3; i++)
	{
		std::string sString;
		if (ExecDosCmd(NULL,lpszCommand,sString,false) && sString.find("ȫ�����")>0)
		{
			for (int j=0; j<5; j++)
			{
				if (copyDir(strToPathTemp.c_str(),strToPath.c_str(),lpszUpdateBackupDir)==1)
				{
					bOK = true;
					break;
				}
				Sleep(1000);
			}
			::deleteDir(strToPathTemp.c_str());		// ɾ����ʱĿ¼
			if (bOK)
			{
				::deleteDir(lpszUpdateBackupDir);		// ɾ������Ŀ¼
			}else
			{
				copyDir(lpszUpdateBackupDir,strToPath.c_str());	// ���Ƹ���ʧ�ܣ���ԭ���ļ���
				::deleteDir(lpszUpdateBackupDir);
				MessageBox(NULL,_T("���Ƹ����ļ�ʧ�ܣ������ԣ�"),_T("�Զ����³���"),MB_OK|MB_ICONWARNING);
			}
			break;
		}
	}

	//if (!bOK)
	//	ShellExecute(NULL,  "open", "Rar.exe", strRarToFiles.c_str(), sAppPath.c_str(), SW_HIDE);
	if (bOK)
		DeleteFile(strRarFile.c_str());

	//ShellExecute(NULL,  "open", "Rar.exe", strRarToFiles.c_str(), sAppPath.c_str(), SW_HIDE);
	//Sleep(1200);
	//while (true)
	//{
	//	 ɾ������ļ�
	//	if (DeleteFile(strRarFile.c_str()))
	//		break;
	//	Sleep(500);
	//}

	// 1���Ƚ�ѹ����ļ�����Ŀ¼
	// 2����ȡ���²����ļ� x.x.x.x.op
	// 3��ִ�в���ָ��
	// 4��������ɾ������ļ��Ͳ���ָ���ļ�

	// ����ִ��������
	ShellExecute(NULL,  "open", strExeName.c_str(), NULL, sAppPath.c_str(), SW_SHOW);
	return 0;
}

