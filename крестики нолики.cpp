#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include  <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
using namespace std;

#define KEY_SHIFTED     0x8000
#define KEY_TOGGLED     0x0001
#define WM_LBUTTONUP    0x0202
#define WM_HOTKEY       0x0312
#define HOTKEY1 0x1000
#define HOTKEY2 0x1002
#define HOTKEY3 0x1003
#define HOTKEY4 0x1004
struct Color
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
};
struct AppConfig
{
    int n;
    int width;
    int height;
    Color cBack;
    Color cLine;
};
#pragma region глобальные переменные
int reg = 0;


const TCHAR szWinClass[] = _T("Win32SampleApp");
const TCHAR szWinName[] = _T("Win32SampleWindow");
HWND hwnd;               /* This is the handle for our window */
HBRUSH hBrush;           /* Current brush */
RECT clientRect;
POINTS pos;

int l_mark = 0;
//int a[n][n];
int* buffer;
int  wid = 0;
int  hig = 0;

//int b[n][n];
int  x = 0, y = 0;
int br_count = 0;
int R = 255, G = 0, B = 0;
int FULL = 0;
UINT mudate;
AppConfig DefaultCFG = { 3,320,240,{0,0,255},{255,0,0} };
AppConfig UserCFG;
HPEN hPen1, hPen2, hPen3, tmp;
int n;
HANDLE Hthread;
HANDLE mutex;
int* turninfo;// 1 - тип хода, 2 - номер хода
UINT endgame = RegisterWindowMessageA("exit");
#pragma endregion


DWORD WINAPI ThreadOfBackground(LPVOID lpParam)
{
    float R = 0;
    float G = 0;
    float B = 0;

    while (1)
    {
        for (int i = 0; i < 155; i++) {

            hBrush = CreateSolidBrush(RGB(0, i, 0));
            HBRUSH hold = (HBRUSH)SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
            DeleteObject(hold);

           Sleep(35);
            InvalidateRect(hwnd, NULL, 1);
        }
        for (int i = 155; i > 0; i--) {

            hBrush = CreateSolidBrush(RGB(0, i, 0));
            HBRUSH hold = (HBRUSH)SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hBrush);
            DeleteObject(hold);

            Sleep(35);
            InvalidateRect(hwnd, NULL, 1);
        }
    }
    return 0;
}
void READ()
{
    fstream f("input.txt");
    nlohmann::json j{};
    if (f)
    {

    }
    else
    {

    }

}
void RunNotepad(void)
{
    STARTUPINFO sInfo;
    PROCESS_INFORMATION pInfo;

    ZeroMemory(&sInfo, sizeof(STARTUPINFO));

    puts("Starting Notepad...");
    CreateProcess(_T("C:\\Windows\\Notepad.exe"),
        NULL, NULL, NULL, FALSE, 0, NULL, NULL, &sInfo, &pInfo);
}


void Draw_display(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, int t)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    GetClientRect(hwnd, &rect);


    double cur1 = 0, cur2 = 0;
    x = rect.right;
    y = rect.bottom;
    wid = rect.right / n;
    hig = rect.bottom / n;


    hdc = BeginPaint(hwnd, &ps);



    hPen2 = CreatePen(PS_SOLID, 10, RGB(255, 0, 255));
    hPen3 = CreatePen(PS_SOLID, 5, RGB(0, 0, 0));


    SelectObject(hdc, hPen1);
    for (int i = 0; i <= n; i++)
    {
        if (i == 0)
        {
            LineTo(hdc, cur1, y);
            MoveToEx(hdc, cur1, cur2, NULL);
            LineTo(hdc, x, cur2);
        }
        else
        {
            cur1 += wid;
            cur2 += hig;
            MoveToEx(hdc, 0, cur2, NULL);
            LineTo(hdc, x, cur2);
            MoveToEx(hdc, cur1, 0, NULL);
            LineTo(hdc, cur1, y);
        }
    }

    // HBRUSH in= hBrush+1;
      /*if (br_count != 0)
          in = hBrush;
      else in = CreateSolidBrush(RGB(0, 0, 255));*/

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
        {
            if (buffer[i * n + j] == 1)
            {
                SelectObject(hdc, hPen2);
                SelectObject(hdc, hBrush);
                Ellipse(
                    hdc,
                    i * wid + 10,
                    j * hig + 10,
                    (i + 1) * wid - 10,
                    (j + 1) * hig - 10);
            }
            if (buffer[i * n + j] == 2)
            {

                SelectObject(hdc, hPen3);
                MoveToEx(hdc, i * wid + 15, j * hig + 15, NULL);
                LineTo(hdc, (i + 1) * wid - 15, (j + 1) * hig - 15);
                MoveToEx(hdc, (i + 1) * wid - 15, j * hig + 15, NULL);
                LineTo(hdc, i * wid + 15, (j + 1) * hig - 15);

            }
        }
    //     br_count= 1;

    DeleteObject(hPen2);
    //  DeleteObject(in);
    DeleteObject(hPen3);
    EndPaint(hwnd, &ps);
}


void ReDraw_BG(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int R = rand() % 255;
    int G = rand() % 255;
    int B = rand() % 255;
    UserCFG.cBack.red = R;
    UserCFG.cBack.blue = B;
    UserCFG.cBack.green = G;
    hBrush = CreateSolidBrush(RGB(R, G, B));
    HBRUSH hold = (HBRUSH)SetClassLongPtrA(hwnd, GCLP_HBRBACKGROUND, (long)hBrush);
    DeleteObject(hold);
}
void ReDraw_Lines(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    GetClientRect(hwnd, &rect);


    double cur1 = 0, cur2 = 0;
    x = rect.right;
    y = rect.bottom;
    wid = rect.right / n;
    hig = rect.bottom / n;


    hdc = BeginPaint(hwnd, &ps);



    EndPaint(hwnd, &ps);


}
void UPDATE_BUFF(int x, int y, int fig)
{
    buffer[x * n + y] = fig;
}
VOID ReadFileCFG() {
    ifstream file;
    file.open("config.txt");
    if (file)
    {
        string s;
        file >> s;
        nlohmann::json p = nlohmann::json::parse(s);
        UserCFG.n = p["n"].get<int>();
        UserCFG.width = p["width"].get<int>();
        UserCFG.height = p["height"].get<int>();

        UserCFG.cBack.red = p["bckR"].get<int>();
        UserCFG.cBack.green = p["bckG"].get<int>();
        UserCFG.cBack.blue = p["bckB"].get<int>();

        UserCFG.cLine.red = p["lineR"].get<int>();
        UserCFG.cLine.green = p["lineG"].get<int>();
        UserCFG.cLine.blue = p["lineB"].get<int>();
    }
    else
        UserCFG = DefaultCFG;
}

VOID WriteFileCFG() {
    nlohmann::json a{};
    a["n"] = UserCFG.n;
    a["width"] = UserCFG.width;
    a["height"] = UserCFG.height;


    a["bckR"] = UserCFG.cBack.red;
    a["bckG"] = UserCFG.cBack.green;
    a["bckB"] = UserCFG.cBack.blue;

    a["lineR"] = UserCFG.cLine.red;
    a["lineG"] = UserCFG.cLine.green;
    a["lineB"] = UserCFG.cLine.blue;
    string serializedString = a.dump();
    ofstream file_out;
    file_out.open("config.txt");
    file_out << serializedString << endl;
    file_out.close();
}
BOOL CollectData() {
    int argc;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

    ReadFileCFG();
    switch (argc)
    {
    case 1:return TRUE;
    case 2: return TRUE;
    case 3: {
        if (_ttoi(argv[2]) > 0)
            UserCFG.n = _ttoi(argv[2]);
        else
            UserCFG.n = DefaultCFG.n;
        return TRUE;
        break;
    }
    default:
        return FALSE;
        break;
    }
}
bool DetermineWinner(int cell)
{
    bool res;
    for (int i = 0; i < UserCFG.n; ++i)
    {
        res = true;
        for (int j = 0; j < UserCFG.n; ++j)
        {
            if (buffer[i * UserCFG.n + j] != cell)
            {
                res = false;
            }
        }
        if (res) return true;
    }

    // вертикали
    for (int j = 0; j < UserCFG.n; ++j)
    {
        res = true;
        for (int i = 0; i < UserCFG.n; ++i)
        {
            if (buffer[i * UserCFG.n + j] != cell)
            {
                res = false;
            }
        }
        if (res) return true;
    }

    // диагонали
    res = true;
    for (int i = 0, j = 0; i < UserCFG.n && j < UserCFG.n; ++i, ++j)
    {
        if (buffer[i * UserCFG.n + j] != cell)
        {
            res = false;
        }
    }
    if (res) return true;

    res = true;
    for (int i = 0, j = UserCFG.n - 1; i < UserCFG.n && j >= 0; ++i, --j)
    {
        if (buffer[i * UserCFG.n + j] != cell)
        {
            res = false;
        }
    }
    if (res) return true;

    return res;
}

void NEXT_MOVE(int move,LPARAM lParam, WPARAM wParam, UINT message)
{
    pos.x = GET_X_LPARAM(lParam);
    pos.y = GET_Y_LPARAM(lParam);
    pos.x = pos.x / wid;
    pos.y = pos.y / hig;
    DWORD condition;
    condition = WaitForSingleObject(mutex, 0);
    if (buffer[pos.x * n + pos.y] == 0)
    {
        if (condition == WAIT_OBJECT_0)
        {
            
            if (turninfo[0] == 0) turninfo[0] = 2;
            if (move == turninfo[0])
            {
                buffer[pos.x * n + pos.y] = move;
                if (turninfo[0] == 2)turninfo[0] = 1;
                else turninfo[0] = 2;
                turninfo[1]++;
            }
            else
            {   if(move==1)
                MessageBoxA(hwnd, "Ход крестиков", "Ошибка", MB_OK);
                if(move==2)
                MessageBoxA(hwnd, "Ход ноликов", "Ошибка", MB_OK);

            }
            if (DetermineWinner(1))
            {
                MessageBoxA(hwnd, "Победа ноликов", "Оповещение", MB_OK);
                PostMessage(HWND_BROADCAST, endgame, wParam, lParam);

            }
            if (DetermineWinner(2))
            {
                MessageBoxA(hwnd, "Победа крестиков", "Оповещение", MB_OK);
                PostMessage(HWND_BROADCAST, endgame, wParam, lParam);

            }
            if (n*n==turninfo[1])
            {
                MessageBoxA(hwnd, "Ничья", "Оповещение", MB_OK);
                PostMessage(HWND_BROADCAST, endgame, wParam, lParam);
            }
            PostMessage(HWND_BROADCAST, mudate, wParam, lParam);
            InvalidateRect(hwnd, 0, TRUE);

            ReleaseMutex(mutex);
        }
        
    }
}
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (reg == 0)
    {

        reg = 1;
    }
    if (message == mudate)
    {      /* for (int i = 0; i < n; i++)
        {
            for (int j = 0; j < n; j++)
            {
                a[i][j] = buffer[i * n + j];
            }
        }*/
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }
    switch (message)                  /* handle the messages */
    {


    case WM_KEYDOWN: //Ракция на клавиатуру
        switch (wParam)
        {
        case VK_SPACE:
            if (ResumeThread(Hthread) == 0)
                SuspendThread(Hthread);
            else
                ResumeThread(Hthread);
            puts("SPACE");
            return 0;
        case VK_NUMPAD1:
            SetThreadPriority(Hthread, THREAD_PRIORITY_IDLE);
            puts("Thread priority bellow normal");
            //cout<< SetThreadPriority(Hthread, THREAD_PRIORITY_BELOW_NORMAL)<<endl;
            std::cout << GetThreadPriority(Hthread) << endl;
            return 0;
        case VK_NUMPAD2:
            SetThreadPriority(Hthread, THREAD_PRIORITY_NORMAL);
            puts("Thread priority normal");
            // cout<< SetThreadPriority(Hthread, THREAD_PRIORITY_NORMAL)<<endl;
            std::cout << GetThreadPriority(Hthread) << endl;
            return 0;
        case VK_NUMPAD3:
            SetThreadPriority(Hthread, THREAD_PRIORITY_ABOVE_NORMAL);
            puts("Thread priority above normal");
            //cout<< SetThreadPriority(Hthread, THREAD_PRIORITY_ABOVE_NORMAL)<<endl;
            std::cout << GetThreadPriority(Hthread) << endl;
            return 0;
        case VK_NUMPAD4:
            SetThreadPriority(Hthread, THREAD_PRIORITY_HIGHEST);
            //cout<< SetThreadPriority(Hthread, THREAD_PRIORITY_HIGHEST)<<endl;
            std::cout << GetThreadPriority(Hthread) << endl;
            puts("Thread priority highest");
            return 0;
        case VK_NUMPAD5:
            SetThreadPriority(Hthread, THREAD_PRIORITY_TIME_CRITICAL);
            //cout<< SetThreadPriority(Hthread, THREAD_PRIORITY_HIGHEST)<<endl;
            std::cout << GetThreadPriority(Hthread) << endl;
            puts("Thread priority CRITICAL");
            return 0;
        }
        return 0;
    case WM_CREATE:
        /*  for (int i = 0; i < n; i++)
          {
              for (int j = 0; j < n; j++)
              {
                  a[i][j] = buffer[i * n + j];
             }
          }*/

        mudate = RegisterWindowMessage(_T("WindowsUpdate"));
        break;

    case WM_SIZE:

        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    case WM_PAINT:
        Draw_display(hwnd, message, wParam, lParam, 0);



        return 0;
    case WM_LBUTTONUP:
        NEXT_MOVE(1, lParam, wParam, message);
      //  puts("click");

      //  pos.x = GET_X_LPARAM(lParam);
       // pos.y = GET_Y_LPARAM(lParam);

       

      //  pos.x = pos.x / wid;
      //  pos.y = pos.y / hig;
      //  if (buffer[pos.x * n + pos.y] == 0)
       //     buffer[pos.x * n + pos.y] = 1;
        //if (a[pos.x][pos.y] == 0)//проверка на наличие фигуры в клетке
        //    a[pos.x][pos.y] = 1;
       /* if (buffer[pos.x * n + pos.y] == 0)
        {
            buffer[pos.x * n + pos.y] = 1;
        }*/


        //UPDATE_BUFF(pos.x, pos.y, a[pos.x] [pos.y]);

       // PostMessage(HWND_BROADCAST, mudate, wParam, lParam);
       /// InvalidateRect(hwnd, 0, TRUE);
       // std::cout << "pos x=" << pos.x + 1 << endl << "pos y=" << pos.y + 1 << endl;

        return 0;

    case WM_RBUTTONUP:
        NEXT_MOVE(2, lParam, wParam, message);

        /* for (int i = 0; i < n; i++)
         {
             for (int j = 0; j < n; j++)
             {
                 a[i][j] = buffer[i * n + j];
             }
         }*/
       // puts("click");
       // pos.x = GET_X_LPARAM(lParam);
       // pos.y = GET_Y_LPARAM(lParam);


      //  pos.x = pos.x / wid;
      //  pos.y = pos.y / hig;
      //  if (buffer[pos.x * n + pos.y] == 0)
       //     buffer[pos.x * n + pos.y] = 2;
        //if (a[pos.x][pos.y] == 0)//проверка на наличие фигуры в клетке
        //    a[pos.x][pos.y] = 2;

        /*if (buffer[pos.x * n + pos.y] == 0)
        {
            buffer[pos.x * n + pos.y] = 2;
        }
      */
      // UPDATE_BUFF(pos.x, pos.y, a[pos.x][pos.y]);
      //  PostMessage(HWND_BROADCAST, mudate, wParam, lParam);
      //  InvalidateRect(hwnd, 0, TRUE);
       // std::cout << "pos x=" << pos.x + 1 << endl << "pos y=" << pos.y + 1 << endl;

        return 0;
    case WM_MOUSEWHEEL:

        /* R = (R +2) % 255;
         G = (G + 4) % 255;
         B = (B + 6) % 255;*/


        for (int i = 0; i < 1; i++)
        {

            if (B != 255) G += 5;
            else  B += 5;
        }
        if (R == G == B == 255) FULL = 1;
        if (FULL)
        {
            for (int i = 0; i < 1; i++)
            {
                if (R != 0) R -= 5;
                else if (G != 0) G -= 5;
                else B -= 5;
            }
        }
        hPen1 = CreatePen(PS_SOLID, 5, RGB(R, G, B));
        UserCFG.cLine.red = R;
        UserCFG.cLine.green = G;
        UserCFG.cLine.blue = B;
        // Draw_display(hwnd, message, wParam, lParam,1);
        InvalidateRect(hwnd, 0, TRUE);

        return 0;
    case WM_HOTKEY:
        switch (wParam)
        {
        case HOTKEY1:
            RunNotepad();
            return 0;
        case HOTKEY2:
            PostQuitMessage(0);
            return 0;
        case HOTKEY3:
            ReDraw_BG(hwnd, message, wParam, lParam);

            InvalidateRect(hwnd, NULL, TRUE);
            return 0;
        case HOTKEY4:
            PostQuitMessage(0);
            return 0;
        }
        return 0;
    case WM_DESTROY:
        RECT pos;
        GetClientRect(hwnd, &pos);
        UserCFG.width = pos.right;
        UserCFG.height = pos.bottom;
        WriteFileCFG();
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        return 0;
    }
    if (message == endgame)
    {
        PostQuitMessage(0);
        return 0;
    }
    /* for messages that we don't deal with */
    return DefWindowProc(hwnd, message, wParam, lParam);
}


int main(int argc, char** argv)
{
    DWORD ThreadID;
    Hthread = CreateThread(THREAD_PRIORITY_NORMAL, 0, ThreadOfBackground, NULL, 0, &ThreadID);
    if (!CollectData())
    {
        MessageBox(NULL, _T("Call to CollectData failed!"), _T("Game"), NULL);
        return EXIT_FAILURE;
    }
    HANDLE lpfilem = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, TEXT("lpfilename"));
    HANDLE lpfilem2 = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, TEXT("lpfilename2"));
    if (lpfilem2 == NULL)
    {
        lpfilem2 = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, 2 * sizeof(int), "lpfilename2");
    }
    turninfo = (int*)MapViewOfFile(lpfilem2, FILE_MAP_ALL_ACCESS, 0, 0, 2 * sizeof(int));
  //  std::fstream f;

    n = UserCFG.n;

   //  mutex = OpenMutexA(MUTEX_ALL_ACCESS, TRUE, "mutex");
    //if (mutex == NULL)
    //{
       // 
   /* }*/
  // mutex = OpenMutexA(SYNCHRONIZE,FALSE,"mutex");
   // if (mutex == NULL)
  //  {
        mutex = CreateMutexA(NULL, FALSE, "mutex");
  //  }
    //Выделяем общую память
    if (lpfilem == NULL)
    {
        lpfilem = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_EXECUTE_READWRITE, 0, UserCFG.n * UserCFG.n * sizeof(int), TEXT("lpfilename"));

        // buffer = (int*)MapViewOfFile(lpfilem, FILE_MAP_ALL_ACCESS, 0, 0, UserCFG.n * UserCFG.n);
    }

    buffer = (int*)MapViewOfFile(lpfilem, FILE_MAP_ALL_ACCESS, 0, 0, UserCFG.n * UserCFG.n * sizeof(int));

    BOOL bMessageOk;
    MSG message;            /* Here message to the application are saved */
    WNDCLASS wincl = { 0 };         /* Data structure for the windowclass */


    /* Harcode show command num when use non-winapi entrypoint */
    int nCmdShow = SW_SHOW;
    /* Get handle */
    HINSTANCE hThisInstance = GetModuleHandle(NULL);

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szWinClass;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by Windows */

    /* Use custom brush to paint the background of the window */
    hBrush = CreateSolidBrush(RGB(UserCFG.cBack.red, UserCFG.cBack.green, UserCFG.cBack.blue));
    hPen1 = CreatePen(PS_SOLID, 5, RGB(UserCFG.cLine.red, UserCFG.cLine.green, UserCFG.cLine.blue));
    wincl.hbrBackground = hBrush;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClass(&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindow(
        szWinClass,          /* Classname */
        szWinName,       /* Title Text */
        WS_OVERLAPPEDWINDOW, /* default window */
        CW_USEDEFAULT,       /* Windows decides the position */
        CW_USEDEFAULT,       /* where the window ends up on the screen */
        UserCFG.width,                 /* The programs width */
        UserCFG.height,                 /* and height in pixels */
        HWND_DESKTOP,        /* The window is a child-window to desktop */
        NULL,                /* No menu */
        hThisInstance,       /* Program Instance handler */
        NULL                 /* No Window Creation data */
    );



    /* Make the window visible on the screen */
    ShowWindow(hwnd, nCmdShow);

    UpdateWindow(hwnd);

    RegisterHotKey(hwnd, HOTKEY1, MOD_SHIFT, 0x43); // shift+c
    RegisterHotKey(hwnd, HOTKEY2, MOD_CONTROL, 0x51); // ctrl+q
    RegisterHotKey(hwnd, HOTKEY3, NULL, VK_RETURN); // enter
    RegisterHotKey(hwnd, HOTKEY4, NULL, VK_ESCAPE); // esc



    /* Run the message loop. It will run until GetMessage() returns 0 */
    while ((bMessageOk = GetMessage(&message, NULL, 0, 0)) != 0)
    {
        /* Yep, fuck logic: BOOL mb not only 1 or 0.
         * See msdn at https://msdn.microsoft.com/en-us/library/windows/desktop/ms644936(v=vs.85).aspx
         */
        if (bMessageOk == -1)
        {
            puts("Suddenly, GetMessage failed! You can call GetLastError() to see what happend");
            break;
        }
        /* Translate virtual-key message into character message */
        TranslateMessage(&message);
        /* Send message to WindowProcedure */
        DispatchMessage(&message);
    }



    /* Cleanup stuff */
    DestroyWindow(hwnd);
    UnregisterClass(szWinClass, hThisInstance);
    DeleteObject(hBrush);
    //  DeleteObject(brush);
    UnregisterHotKey(hwnd, HOTKEY1);
    UnregisterHotKey(hwnd, HOTKEY2);
    UnregisterHotKey(hwnd, HOTKEY3);
    UnregisterHotKey(hwnd, HOTKEY4);
    DeleteObject(hPen1);
    UnmapViewOfFile(buffer);
    CloseHandle(lpfilem);
    CloseHandle(Hthread);
    // return 0;
}

