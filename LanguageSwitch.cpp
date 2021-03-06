// ConsoleApplication3.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <cstdlib>
#include <string>
#include <psapi.h>
#include <algorithm>
#ifdef WINVER
#undef WINVER
#endif
#define WINVER 0x500

#define NUMOFAPPS 7
std::string ChineseProcesses[NUMOFAPPS] = { "WeChat.exe", "QQ.exe","Xiami.exe","notepad.exe","WINWORD.EXE","chrome.EXE","DingTalk.EXE" };

bool IsChineseProcess(std::string progName)
{
  std::transform(progName.begin(), progName.end(), progName.begin(), ::tolower);
  for (size_t i = 0; i < NUMOFAPPS; i++)
  {
    if (progName == ChineseProcesses[i])
      return true;
  }

  return false;
}
// Create a string with last error message
std::string GetLastErrorStdStr()
{
  DWORD error = GetLastError();
  if (error)
  {
    LPVOID lpMsgBuf;
    DWORD bufLen = FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      error,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR)&lpMsgBuf,
      0, NULL);
    if (bufLen)
    {
      LPCSTR lpMsgStr = (LPCSTR)lpMsgBuf;
      std::string result(lpMsgStr, lpMsgStr + bufLen);

      LocalFree(lpMsgBuf);

      return result;
    }
  }
  return std::string();
}

HKL g_hklRPC = NULL;
const HKL ENG = (HKL)67699721;
const HKL CHS = (HKL)134481924;
//BOOL InitHklRPC()
//{
//  // Find RPC hkl  
//  UINT nMaxHklCnt = GetKeyboardLayoutList(0, NULL);
//
//  HKL* p_hklList = (HKL*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nMaxHklCnt * sizeof(HKL));
//
//  UINT nHklCnt = GetKeyboardLayoutList(nMaxHklCnt, p_hklList);
//
//  for (UINT i = 0; i<nHklCnt; i++) {
//    if (LOWORD(p_hklList[i]) != 0x0804) {
//      g_hklRPC = p_hklList[i];
//      break;
//    }
//  }
//
//  HeapFree(GetProcessHeap(), NULL, p_hklList);
//
//  return (g_hklRPC == NULL) ? FALSE : TRUE;
//}

std::string getProcessName(std::string fullpath)
{
  for (int i = fullpath.size() - 1; i >= 0; i--)
  {
    char c = fullpath[i];
    if (c == '/' || c == '\\') {
      return fullpath.substr(i + 1, fullpath.size() - 1 - i);
    }
  }

  return fullpath;

}
char filenameBuffer[4000];
// MyTaskBarAddIcon - adds an icon to the taskbar notification area. 
// Returns TRUE if successful or FALSE otherwise. 
// hwnd - handle of the window to receive callback messages 
// uID - identifier of the icon 
// hicon - handle of the icon to add 
// lpszTip - tooltip text 
//BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip)
//{
//  BOOL res;
//  NOTIFYICONDATA tnid;
//
//  tnid.cbSize = sizeof(NOTIFYICONDATA);
//  tnid.hWnd = hwnd;
//
//  tnid.uID = uID;
//  tnid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;
//  tnid.uCallbackMessage = MYWM_NOTIFYICON;
//  tnid.hIcon = hicon;
//  if (lpszTip)
//    lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip));
//  else
//    tnid.szTip[0] = '\0';
//
//  res = Shell_NotifyIcon(NIM_ADD, &tnid);
//
//  if (hicon)
//    DestroyIcon(hicon);
//
//  return res;
//}
//
//
////To delete an icon from the taskbar notification area, you must fill a NOTIFYICONDATA structure and send it to the system in the context of an NIM_DELETE message.When deleting a taskbar icon, you need to specify only the cbSize, hWnd, and uID members, as the following example shows.
//
//// MyTaskBarDeleteIcon - deletes an icon from the taskbar 
////     notification area. 
//// Returns TRUE if successful or FALSE otherwise. 
//// hwnd - handle of the window that added the icon 
//// uID - identifier of the icon to delete 
//BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID)
//{
//  BOOL res;
//  NOTIFYICONDATA tnid;
//
//  tnid.cbSize = sizeof(NOTIFYICONDATA);
//  tnid.hWnd = hwnd;
//  tnid.uID = uID;
//
//  res = Shell_NotifyIcon(NIM_DELETE, &tnid);
//  return res;
//
//}

int main()
{

  for (size_t i = 0; i < NUMOFAPPS; i++)
  {
    std::string str = ChineseProcesses[i];
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    ChineseProcesses[i] = str;
  }

  HWND myHandle = GetConsoleWindow();
  ShowWindow(myHandle, 0);

  while (true)
  {
    HWND hWnd = GetForegroundWindow();
    //ASSERT(IsWindow(hWnd))
    // Get Target Thread ID and Attach Thread Input  
    DWORD ProcID;
    DWORD ThreadID = GetWindowThreadProcessId(hWnd, &ProcID);
    HKL cur_key_locale = GetKeyboardLayout(ThreadID);
    //if (cur_key_locale == ENG)
    //  continue;
    HMODULE hMods[1024];
    //HANDLE ProcHandle = GetCurrentProcess();
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, ProcID);
    std::string progName = "";
    if (GetModuleFileNameExA(hProcess, NULL, filenameBuffer, 4000))
    {
      progName = getProcessName(filenameBuffer);
      printf("%s,%d\n", progName.data(), (unsigned long)cur_key_locale);
    }

    if (cur_key_locale == ENG)
    {
      Sleep(100);
      continue;
    }

    bool isChineseProg = IsChineseProcess(progName);
    if (isChineseProg)
    {
      Sleep(100);
      continue;
    }


    AttachThreadInput(GetCurrentThreadId(), ThreadID, TRUE);
    // Get Target Window  
    HWND hWndFocus = GetFocus();
    // Post Input Messages   
    // ASSERT(IsWindow(hWndFocus));

    // Change Input Language  
    PostMessage(hWndFocus, WM_INPUTLANGCHANGEREQUEST, (WPARAM)TRUE, (LPARAM)ENG);   // Use Language ID 0x0804(Chinese PRC)  
                                                                                    // Detach Thread Input  
    AttachThreadInput(GetCurrentThreadId(), ThreadID, FALSE);

    // printf("%d,%d,%d\n", g_hklRPC, ENG, CHS);
    Sleep(100);
  }

  //WCHAR szBuf[25];
  //double thId;
  //HANDLE hCurrentKBLayout;
  //int i = 0;
  //HWND  _curr_window = GetForegroundWindow();
  //DWORD _curr_window_procces_id;
  //DWORD _curr_window_thread_id = GetWindowThreadProcessId(_curr_window, &_curr_window_procces_id);
  ////std::cout << "Process ID is " << _curr_window_procces_id << " and Thread ID is " << _curr_window_thread_id << std::endl;
  //HKL _key_locale = GetKeyboardLayout(_curr_window_thread_id);
  //unsigned long val = (unsigned long)_key_locale;
  //printf("%d,%d,%d,%d\n", _curr_window, _curr_window_procces_id, _curr_window_thread_id, val);
  ////if (val != ENG)
  ////{
  //  //ActivateKeyboardLayout((HKL)ENG, KLF_REORDER);
  //  ActivateKeyboardLayout(HKL(HKL_PREV), KLF_REORDER);
  // //printf("%s\n",GetLastErrorStdStr().data());

  // // HWND hWnd = GetForegroundWindow();
  //  //ASSERT(IsWindow(hWnd))

  //    // Get Target Thread ID and Attach Thread Input  
  //  //  DWORD ProcID;
  //  //DWORD ThreadID = GetWindowThreadProcessId(hWnd, &ProcID);
  //  //AttachThreadInput(GetCurrentThreadId(), ThreadID, TRUE);

  //  // Get Target Window  
  //  HWND hWndFocus = GetFocus();

  //  // Post Input Messages   
  // // ASSERT(IsWindow(hWndFocus));

  //  // Change Input Language  
  //  PostMessage(hWndFocus, WM_INPUTLANGCHANGEREQUEST, (WPARAM)TRUE, (LPARAM)((HKL)ENG));   // Use Language ID 0x0804(Chinese PRC)  

  //                                                                                       // Detach Thread Input  
  //  AttachThreadInput(GetCurrentThreadId(), _curr_window_thread_id, FALSE);

  //}
  //printf("%d,%d,%d,%d\n", _curr_window, _curr_window_procces_id, _curr_window_thread_id, val);
  //LoadKeyboardLayout()
  //std::cout << "Keyboard layout is " << _key_locale << std::endl;
  //i++;
  //if (i % 1000000 == 0)
  //{
  // GUITHREADINFO Gti;
  // ::ZeroMemory(&Gti, sizeof(GUITHREADINFO));
  //Gti.cbSize = sizeof(GUITHREADINFO);

  //::GetGUIThreadInfo(0, &Gti);
  // DWORD dwThread = ::GetWindowThreadProcessId(Gti.hwndActive, 0);

  //HKL lang = ::GetKeyboardLayout(dwThread);
  //printf("%d\n", lang);
  //}
  //if (String(szBuf).Length() == 0)
  // strcpy(szBuf, "系统默认");
  //ShowMessage("current input language is:" + String(szBuf));
  return 0;
}

