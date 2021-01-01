/*	Benjamin DELPY `gentilkiwi`
	https://blog.gentilkiwi.com
	benjamin@gentilkiwi.com
	Licence : https://creativecommons.org/licenses/by/4.0/
*/
#include "kuhl_m_process.h"

BOOL kull_m_process_run_data(LPCWSTR commandLine, HANDLE hToken)
{
	BOOL status = FALSE;
	SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES), NULL, TRUE };
	STARTUPINFO si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	HANDLE hOut = NULL;
	PWSTR dupCommandLine = NULL;
	BYTE chBuf[4096];
	DWORD dwRead, i;
	LPVOID env = NULL;

	if (dupCommandLine = _wcsdup(commandLine))
	{
		if (CreatePipe(&hOut, &si.hStdOutput, &saAttr, 0))
		{
			SetHandleInformation(hOut, HANDLE_FLAG_INHERIT, 0);
			si.cb = sizeof(STARTUPINFO);
			si.hStdError = si.hStdOutput;
			si.dwFlags |= STARTF_USESTDHANDLES;
			if (!hToken || CreateEnvironmentBlock(&env, hToken, FALSE))
			{
				if (status = CreateProcessAsUser(hToken, NULL, dupCommandLine, NULL, NULL, TRUE, CREATE_NO_WINDOW | CREATE_UNICODE_ENVIRONMENT, env, NULL, &si, &pi))
				{
					CloseHandle(si.hStdOutput);
					si.hStdOutput = si.hStdError = NULL;
					while (ReadFile(hOut, chBuf, sizeof(chBuf), &dwRead, NULL) && dwRead)
						for (i = 0; i < dwRead; i++)
							kprintf(L"%c", chBuf[i]);
					WaitForSingleObject(pi.hProcess, INFINITE);
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
				}
				else PRINT_ERROR_AUTO(L"CreateProcessAsUser");
				if (env)
					DestroyEnvironmentBlock(env);
			}
			else PRINT_ERROR_AUTO(L"CreateEnvironmentBlock");
			CloseHandle(hOut);
			if (si.hStdOutput)
				CloseHandle(si.hStdOutput);
		}
		free(dupCommandLine);
	}
	return status;
}