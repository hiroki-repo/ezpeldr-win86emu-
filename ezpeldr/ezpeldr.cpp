// ezpeldr.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <windows.h>
#include <stdlib.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#include <iostream>


DWORD MemSize = 4 * 1024 * 1024;	// reserve 4Mb by default
DWORD Base = 0x400000; //0x1000000

#ifdef _X86_
static char volatile Buff[16 * 1024 * 1024];
#endif

int main()
{
#ifdef _X86_
    Buff[16 * 1024 * 1024 - 1] = (char)GetTickCount();
#endif
    //std::cout << "Hello World!\n";
    wchar_t* ExePath;
    size_t Len1;
    _wdupenv_s(&ExePath, &Len1, L"EMU_PROGRAM");
    while (ExePath[0] == ' ' || ExePath[0] == '\"')
        ExePath++;
    while (wcslen(ExePath) && (ExePath[wcslen(ExePath) - 1] == '\"' || ExePath[wcslen(ExePath) - 1] == ' '))
        ExePath[wcslen(ExePath) - 1] = 0;

#if 0 //def _DEBUG
    _CrtSetDbgFlag(
        _CRTDBG_ALLOC_MEM_DF |
        _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR,
        _CRTDBG_MODE_DEBUG);
#endif 
    /*	HANDLE H=CreateFile(_wgetenv(L"EMU_PROGRAM"),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,0,OPEN_EXISTING,0,0);
        if(H==INVALID_HANDLE_VALUE)
        {
        puts("EMU_PROGRAM env var is incorrect");
        return 4;
        }
        int Tmp=GetFileSize(H,0);
        if(MemSize<Tmp)
        MemSize=Tmp;
        CloseHandle(H);
        MemSize=(MemSize+65535)&~65535;	// align on 64K
        */
#if 0
#ifdef _ARM_
    HANDLE H = CreateFileW(ExePath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (H == INVALID_HANDLE_VALUE)
        return 0xBAD00001;

    // Validate PE
    char Buff1[512];
    DWORD Len = 0;
    if (ReadFile(H, Buff1, 512, &Len, 0) == FALSE || Len != 512 || Buff1[0] != 'M' || Buff1[1] != 'Z')
    {
        CloseHandle(H);
        return 0xBAD00002;
    }
    if (SetFilePointer(H, ((PIMAGE_DOS_HEADER)Buff1)->e_lfanew, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
    {
        CloseHandle(H);
        return 0xBAD00002;
    }
    if (ReadFile(H, Buff1, 512, &Len, 0) == FALSE || Len != 512 || Buff1[0] != 'P' || Buff1[1] != 'E' || Buff1[2] != 0 || Buff1[3] != 0)
    {
        CloseHandle(H);
        return 0xBAD00002;
    }
    if (Buff1[5] != 1 || Buff1[4] != 0x4c)
    {
        CloseHandle(H);
        return 0xBAD00002;
    }

    PIMAGE_NT_HEADERS32 nt = (PIMAGE_NT_HEADERS32)Buff1;

    if ((nt->FileHeader.Characteristics & IMAGE_FILE_RELOCS_STRIPPED) || nt->OptionalHeader.MajorOperatingSystemVersion < 5)
    {
        Base = nt->OptionalHeader.ImageBase;
        MemSize = nt->OptionalHeader.SizeOfImage;

        MemSize = (MemSize + 65535);
        MemSize /= 65536;
        MemSize *= 65536;

        void* Ptr = VirtualAlloc((void*)nt->OptionalHeader.ImageBase, MemSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

        if (Ptr == 0)
        {
#ifdef _DEBUG
            printf("Failed to reserve memory (0x%08x, %d), please rerun the application\n", nt->OptionalHeader.ImageBase, MemSize);
#endif
            return 0xBAD00000;
        }
    }

#endif
#endif

    HMODULE peldr = LoadLibrary(L"peldr.nt.dll");
    if (peldr == 0)
    {
        //		SetCursor(OldCur);
        puts("Can't load peldr dll");
        return 0xBAD00003;
    }

    typedef int t_PeLdrStartProgram(LPWSTR ExePath);

    t_PeLdrStartProgram* start = (t_PeLdrStartProgram*)GetProcAddress(peldr, "PeLdrStartProgram");
    if (start == 0)
    {
        //		SetCursor(OldCur);
        puts("Can't find start program proc");
        return 0xBAD00003;
    }

    int ret = start(ExePath);
    //	SetCursor(OldCur);
    return ret;
}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
