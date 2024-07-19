#include "framework.h"
#include "Hero and Monsters.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "ErrH.h"
#include "FCheck.h"
#include "D2BMPLOADER.h"
#include "Engine.h"
#include <vector>
#include <fstream>
#include <chrono>

#pragma comment (lib, "winmm.lib")
#pragma comment (lib, "d2d1.lib")
#pragma comment (lib, "dwrite.lib")
#pragma comment (lib, "errh.lib")
#pragma comment (lib, "fcheck.lib")
#pragma comment (lib, "d2bmploader.lib")
#pragma comment (lib, "engine.lib")

#define tmp_file ".\\res\\data\\temp.dat"
#define Ltmp_file L".\\res\\data\\temp.dat"
#define snd_file L".\\res\\snd\\main.wav"
#define record_file L".\\res\\data\\record.dat"
#define save_file L".\\res\\data\\save.dat"
#define help_file L".\\res\\data\\help.dat"

#define bWinClassName L"CreaturesDemolish"
#define mNew 1001
#define mExit 1002
#define mSave 1003
#define mLoad 1004
#define mHoF 1005

#define record 2001
#define first_record 2002
#define no_record 2003

WNDCLASS bWin = { 0 };
HINSTANCE bIns = nullptr;
HWND bHwnd = nullptr;
HICON mainIcon = nullptr;
HCURSOR mainCur = nullptr;
HCURSOR outCur = nullptr;
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
HDC PaintDC = nullptr;
PAINTSTRUCT bPaint = { 0 };
POINT cur_pos = { 0 };
UINT bTimer = 0;
MSG bMsg = { 0 };
BOOL bRet = 0;

D2D1_RECT_F b1Rect = { 0, 0, scr_width / 3 - 30.0f, 50.0f };
D2D1_RECT_F b2Rect = { scr_width / 3, 0, scr_width * 2 / 3 - 30.0f, 50.0f };
D2D1_RECT_F b3Rect = { scr_width * 2 / 3 , 0, scr_width, 50.0f };

D2D1_RECT_F b1TxtRect = { 30.0f, 0, scr_width / 3 - 60.0f, 50.0f };
D2D1_RECT_F b2TxtRect = { scr_width / 3 + 30.0f, 0, scr_width * 2 / 3 - 60.0f, 50.0f };
D2D1_RECT_F b3TxtRect = { scr_width * 2 / 3 + 30.0f, 0, scr_width - 30.0f, 50.0f };

D2D1_RECT_F damage_area = { 0 };

wchar_t current_player[16] = L"ONE HERO";

bool pause = false;
bool sound = true;
bool show_help = false;
bool name_set = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;
bool in_client = true;

bool hero_killed = false;
int hero_killed_delay = 400;
float hero_killed_x = 0;

bool evil_killed = false;
int evil_killed_delay = 500;
float evil_killed_x = 0;

bool damage_area_set = false;
bool evil_up_shield = false;

int score = 0;
int mins = 0;
int secs = 0;

////////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

ID2D1RadialGradientBrush* butBckBr = nullptr;
ID2D1SolidColorBrush* TxtBr = nullptr;
ID2D1SolidColorBrush* HglTxtBr = nullptr;
ID2D1SolidColorBrush* InactTxtBr = nullptr;

ID2D1SolidColorBrush* LifeBr = nullptr;
ID2D1SolidColorBrush* HurtBr = nullptr;
ID2D1SolidColorBrush* CritBr = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmText = nullptr;
IDWriteTextFormat* bigText = nullptr;

ID2D1Bitmap* bmpField = nullptr;
ID2D1Bitmap* bmpSky = nullptr;

ID2D1Bitmap* bmpHero = nullptr;
ID2D1Bitmap* bmpHeroShot = nullptr;
ID2D1Bitmap* bmpHeroShield = nullptr;

ID2D1Bitmap* bmpEvilShot = nullptr;
ID2D1Bitmap* bmpEvilShield = nullptr;
ID2D1Bitmap* bmpCentaur1 = nullptr;
ID2D1Bitmap* bmpCentaur2 = nullptr;
ID2D1Bitmap* bmpCrock = nullptr;
ID2D1Bitmap* bmpGargoyle1 = nullptr;
ID2D1Bitmap* bmpGargoyle2 = nullptr;
ID2D1Bitmap* bmpOctopus1 = nullptr;
ID2D1Bitmap* bmpOctopus2 = nullptr;
ID2D1Bitmap* bmpTree = nullptr;
ID2D1Bitmap* bmpRip = nullptr;
/////////////////////////////////////////////////

dll::Hero Warrior = nullptr;
dll::Creature Evil = nullptr;

dll::Atom GoodShield = nullptr;
dll::Atom EvilShield = nullptr;

std::vector<dll::Shot>vGoodShots;
std::vector<dll::Shot>vEvilShots;

/////////////////////////////////////////////////

template<typename GARBAGE> bool CleanUp(GARBAGE** what)
{
    if ((*what))
    {
        (*what)->Release();
        (*what) = nullptr;
        return true;
    }
    return false;
}
void LogError(LPCWSTR what)
{
    std::wofstream log(L".\\res\\data\\error.log", std::ios::app);
    log << what << L" ! Time of occurence: " << std::chrono::system_clock::now() << std::endl;
    log.close();
}
void ReleaseResources()
{
    if (!CleanUp(&iFactory))LogError(L"Error releasing memory for iFactory");
    if (!CleanUp(&Draw))LogError(L"Error releasing memory for HWND Render Target");
    if (!CleanUp(&butBckBr))LogError(L"Error releasing memory for Background brush");
    if (!CleanUp(&TxtBr))LogError(L"Error releasing memory for text brush");
    if (!CleanUp(&HglTxtBr))LogError(L"Error releasing memory for highlighted text brush");
    if (!CleanUp(&InactTxtBr))LogError(L"Error releasing memory for inactive text brush");

    if (!CleanUp(&LifeBr))LogError(L"Error releasing memory for Life brush");
    if (!CleanUp(&CritBr))LogError(L"Error releasing memory for Crit brush");
    if (!CleanUp(&HurtBr))LogError(L"Error releasing memory for Hurt brush");

    if (!CleanUp(&iWriteFactory))LogError(L"Error releasing memory for Write Factory");
    if (!CleanUp(&nrmText))LogError(L"Error releasing memory for normal text format");
    if (!CleanUp(&bigText))LogError(L"Error releasing memory for big text format");

    if (!CleanUp(&bmpField))LogError(L"Error releasing memory for bmpField");
    if (!CleanUp(&bmpSky))LogError(L"Error releasing memory for bmpSky");
    
    if (!CleanUp(&bmpHero))LogError(L"Error releasing memory for bmpHero");
    if (!CleanUp(&bmpHeroShot))LogError(L"Error releasing memory for bmpHeroShot");
    if (!CleanUp(&bmpHeroShield))LogError(L"Error releasing memory for bmpHeroShield");

    if (!CleanUp(&bmpEvilShot))LogError(L"Error releasing memory for bmpEvilShot");
    if (!CleanUp(&bmpEvilShield))LogError(L"Error releasing memory for bmpEvilShield");

    if (!CleanUp(&bmpCentaur1))LogError(L"Error releasing memory for bmpCentaur1");
    if (!CleanUp(&bmpCentaur2))LogError(L"Error releasing memory for bmpCentaur2");
    if (!CleanUp(&bmpCrock))LogError(L"Error releasing memory for bmpCrock");
    if (!CleanUp(&bmpGargoyle1))LogError(L"Error releasing memory for bmpGargoyle1");
    if (!CleanUp(&bmpGargoyle2))LogError(L"Error releasing memory for bmpGargoyle2");
    if (!CleanUp(&bmpOctopus1))LogError(L"Error releasing memory for bmpOctopus1");
    if (!CleanUp(&bmpOctopus2))LogError(L"Error releasing memory for bmpOctopus2");
    if (!CleanUp(&bmpTree))LogError(L"Error releasing memory for bmpTree");
    if (!CleanUp(&bmpRip))LogError(L"Error releasing memory for bmpRip");
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    ReleaseResources();
    std::remove(tmp_file);
    exit(1);
}
void InitGame()
{
    score = 0;
    wcscpy_s(current_player, L"ONE HERO");
    name_set = false;
    mins = 0;
    secs = 0;

    if (Warrior)
    {
        CleanUp(&Warrior);
        Warrior = nullptr;
    }
    if (Evil)
    {
        CleanUp(&Evil);
        Evil = nullptr;
    }

    if (GoodShield)
    {
        GoodShield->ReleaseHeapAtom();
        GoodShield = nullptr;
    }
    if (EvilShield)
    {
        EvilShield->ReleaseHeapAtom();
        EvilShield = nullptr;
    }

    if (!vGoodShots.empty())
    {
        for (int i = 0; i < vGoodShots.size(); i++)vGoodShots[i]->Release();
    }
    vGoodShots.clear();

    if (!vEvilShots.empty())
    {
        for (int i = 0; i < vEvilShots.size(); i++)vEvilShots[i]->Release();
    }
    vEvilShots.clear();

    Warrior = reinterpret_cast<dll::Hero>(dll::CreatureFactory(types::hero, 100.0f));
}
BOOL CheckRecord()
{
    if (score < 1) return no_record;

    int result = 0;
    CheckFile(record_file, &result);
    if (result == FILE_NOT_EXIST)
    {
        std::wofstream rec(record_file);
        rec << score << std::endl;
        for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
        rec.close();
        return first_record;
    }
    else
    {
        std::wifstream check(record_file);
        check >> result;
        check.close();
        if (score > result)
        {
            std::wofstream rec(record_file);
            rec << score << std::endl;
            for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
            rec.close();
            return record;
        }
    }

    return no_record;
}
void GameOver()
{
    KillTimer(bHwnd, bTimer);
    PlaySound(NULL, NULL, NULL);
    if (secs > 120)score += 5 * secs;

    wchar_t fin_txt[27] = L"ФАНТАСМАГОРИИТЕ ПОБЕДИХА !";
    int txt_size = 27;

    switch (CheckRecord())
    {
    case no_record:
        if (sound)PlaySound(L".\\res\\snd\\loose.wav", NULL, SND_ASYNC);
        break;

    case first_record:
        if (sound)PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(fin_txt, L"ПЪРВИ РЕКОРД НА ИГРАТА !");
        txt_size = 25;
        break;

    case record:
        if (sound)PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(fin_txt, L"НОВ РЕКОРД НА ИГРАТА !");
        txt_size = 23;
        break;
    }

    Draw->BeginDraw();
    Draw->Clear(D2D1::ColorF(D2D1::ColorF::SandyBrown));
    if (bigText && TxtBr)
        Draw->DrawTextW(fin_txt, txt_size, bigText, D2D1::RectF(50.0f, scr_height / 2 - 150.0f,
            scr_width, scr_height), TxtBr);
    Draw->EndDraw();
    Sleep(6800);

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}
void ShowRecord()
{
    int result = 0;
    CheckFile(record_file, &result);
    if (result == FILE_NOT_EXIST)
    {
        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Все още няма рекорд на играта !\n\nПостарай се повече !",
            L"Липсва файл !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
        return;
    }

    wchar_t stat[150] = L"НАЙ-СМЕЛ ВОЙНИК: ";
    wchar_t saved_player[16] = L"\0";
    wchar_t add[5] = L"\0";
    int txt_size = 0;

    std::wifstream rec(record_file);
    rec >> result;
    wsprintf(add, L"%d", result);
    for (int i = 0; i < 16; i++)
    {
        int letter = 0;
        rec >> letter;
        saved_player[i] = static_cast<wchar_t>(letter);
    }
    wcscat_s(stat, saved_player);
    wcscat_s(stat, L"\n\nСВЕТОВЕН РЕКОРД: ");
    wcscat_s(stat, add);
    rec.close();

    for (int i = 0; i < 150; i++)
    {
        if (stat[i] != '\0') txt_size++;
        else break;
    }

    if (sound)mciSendString(L"play .\\res\\snd\\showrec.wav", NULL, NULL, NULL);
    Draw->BeginDraw();
    Draw->Clear(D2D1::ColorF(D2D1::ColorF::SandyBrown));
    if (bigText && TxtBr)
        Draw->DrawTextW(stat, txt_size, bigText, D2D1::RectF(10.0f, 100.0f, scr_width, scr_height), TxtBr);
    Draw->EndDraw();
    Sleep(4000);

}
void SaveGame()
{
    int result = 0;
    CheckFile(save_file, &result);
    if (result == FILE_EXIST)
    {
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(bHwnd, L"Съществува предишна игра, която ще бъде загубена !\n\nНаистина ли да я презапиша ?",
            L"Презапис !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO) return;
    }

    std::wofstream save(save_file);

    save << score << std::endl;
    for (int i = 0; i < 16; i++)save << static_cast<int>(current_player[i]) << std::endl;
    save << name_set << std::endl;
    save << secs << std::endl;

    save << hero_killed << std::endl;
    if (!hero_killed)
    {
        save << Warrior->x << std::endl;
        save << Warrior->lifes << std::endl;
    }

    if (!Evil)save << 0 << std::endl;
    else
    {
        save << 1 << std::endl;
        save << static_cast<int>(Evil->GetType()) << std::endl;
        save << Evil->x << std::endl;
        save << Evil->lifes << std::endl;
    }
    save.close();

    if (sound)mciSendString(L"play .\\res\\snd\\save.wav", NULL, NULL, NULL);

    MessageBox(bHwnd, L"Играта е запазена !", L"Запис !", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
}
void LoadGame()
{
    int result = 0;
    CheckFile(save_file, &result);
    if (result == FILE_EXIST)
    {
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(bHwnd, L"Настоящата игра ще бъде загубена !\n\nНаистина ли да я презапиша ?",
            L"Презапис !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO) return;
    }
    else
    {
        if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
        MessageBox(bHwnd, L"Все още няма записана игра !\n\nПостарай се повече !",
            L"Липсва файл !", MB_OK | MB_APPLMODAL | MB_ICONERROR);
        return;
    }

    if (Warrior)
    {
        CleanUp(&Warrior);
        Warrior = nullptr;
    }
    if (Evil)
    {
        CleanUp(&Evil);
        Evil = nullptr;
    }

    if (GoodShield)
    {
        GoodShield->ReleaseHeapAtom();
        GoodShield = nullptr;
    }
    if (EvilShield)
    {
        EvilShield->ReleaseHeapAtom();
        EvilShield = nullptr;
    }

    if (!vGoodShots.empty())
    {
        for (int i = 0; i < vGoodShots.size(); i++)vGoodShots[i]->Release();
    }
    vGoodShots.clear();

    if (!vEvilShots.empty())
    {
        for (int i = 0; i < vEvilShots.size(); i++)vEvilShots[i]->Release();
    }
    vEvilShots.clear();

    std::wifstream save(save_file);

    save >> score;
    for (int i = 0; i < 16; i++)
    {
        int letter = 0;
        save >> letter; 
        current_player[i] = static_cast<wchar_t>(letter);
    }
    save >> name_set;
    save >> secs;

    save >> hero_killed;
    if (!hero_killed)
    {
        float wx = 0;
        save >> wx;
        save >> result;

        Warrior = reinterpret_cast<dll::Hero>(dll::CreatureFactory(types::hero, wx));
        Warrior->lifes = result;
    }
    else GameOver();

    save >> result;

    if (result != 0)
    {
        float ax = 0;
        int atype = 0;

        save >> atype;
        save >> ax;
        save >> result;

        Evil = reinterpret_cast<dll::Creature>(dll::CreatureFactory(static_cast<types>(atype), ax));
        Evil->lifes = result;
    }
    save.close();

    if (sound)mciSendString(L"play .\\res\\snd\\save.wav", NULL, NULL, NULL);

    MessageBox(bHwnd, L"Играта е заредена !", L"Зареждане !", MB_OK | MB_APPLMODAL | MB_ICONINFORMATION);
}
void ShowHelp()
{
    int result = 0;
    CheckFile(save_file, &result);
    if (result == FILE_EXIST)
    {
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        MessageBox(bHwnd, L"Липсва помощ за играта !\n\nСвържете се с разработчика !",
            L"Липсва файл !", MB_OK | MB_APPLMODAL | MB_ICONQUESTION); 
        return;
    }

    wchar_t help_txt[1000] = L"\0";
    std::wifstream help(help_file);
    help >> result;
    for (int i = 0; i < result; i++)
    {
        int letter = 0;
        help >> letter;
        help_txt[i] = static_cast<wchar_t>(letter);
    }
    help.close();

    if (sound)mciSendString(L"play .\\res\\snd\\help.wav", NULL, NULL, NULL);
    Draw->BeginDraw();
    Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkGray));
    if (nrmText && bigText && butBckBr && TxtBr && InactTxtBr && HglTxtBr)
    {
        Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), butBckBr);
        if (name_set)
            Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, InactTxtBr);
        else if (b1Hglt)
            Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, HglTxtBr);
        else
            Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, TxtBr);
        if (b2Hglt)
            Draw->DrawTextW(L"ЗВУЦИ ON /OFF", 14, nrmText, b2TxtRect, HglTxtBr);
        else
            Draw->DrawTextW(L"ЗВУЦИ ON /OFF", 14, nrmText, b2TxtRect, TxtBr);
        if (b3Hglt)
            Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, HglTxtBr);
        else
            Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, TxtBr);
    }
    if (nrmText && TxtBr)
        Draw->DrawTextW(help_txt, result, nrmText, D2D1::RectF(100.0f, 150.0f, scr_width, scr_height), TxtBr);
    Draw->EndDraw();
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(mainIcon));
        return true;
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;
        
        case IDOK:
            if (GetDlgItemTextW(hwnd, IDC_NAME, current_player, 16) < 1)
            {
                if (sound)mciSendString(L"play .\\res\\negative.wav", NULL, NULL, NULL);
                MessageBox(bHwnd, L"Ха, ха, хааа ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                wcscpy_s(current_player, L"ONE HERO");
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
        }
        break;
    }
    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK WinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        SetTimer(hwnd, bTimer, 1000, NULL);
        srand((unsigned int)(time(0)));
       
        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();

        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
        AppendMenu(bBar, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");

        AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLoad, L"Зареди игра");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");

        SetMenu(hwnd, bBar);
        InitGame();
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(hwnd, L"Ако излезеш, ще загубиш тази игра !\n\nНаистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(100, 100, 100)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        secs++;
        mins = secs / 60;
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1TxtRect.left && cur_pos.x <= b1TxtRect.right)
                {
                    if (!b1Hglt || b2Hglt || b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                        b2Hglt = false;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b2TxtRect.left && cur_pos.x <= b2TxtRect.right)
                {
                    if (!b2Hglt || b1Hglt || b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = true;
                        b3Hglt = false;
                    }
                }
                if (cur_pos.x >= b3TxtRect.left && cur_pos.x <= b3TxtRect.right)
                {
                    if (b1Hglt || b2Hglt || !b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = false;
                        b2Hglt = false;
                        b3Hglt = true;
                    }
                }
                SetCursor(outCur);
                return true;
            }
            else
            {
                if (b1Hglt || b2Hglt || b3Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1Hglt = false;
                    b2Hglt = false;
                    b3Hglt = false;
                }
                SetCursor(mainCur);
                return true;
            }
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }
            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Ако почнеш отначало, ще загубиш тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;

        case mSave:
            pause = true;
            SaveGame();
            pause = false;
            break;

        case mLoad:
            pause = true;
            LoadGame();
            pause = false;
            break;

        case mHoF:
            pause = true;
            ShowRecord();
            pause = false;
            break;

        }
        break;

    case WM_KEYDOWN:
        if (Warrior)
        {
            switch (LOWORD(wParam))
            {
            case VK_RIGHT:
                if (Warrior->current_action != actions::block)Warrior->Move(true);
                break;

            case VK_LEFT:
                if (Warrior->current_action != actions::block)Warrior->Move(false);
                break;

            case VK_SHIFT:
                if (Warrior->current_action != actions::block)Warrior->Block();
                break;
            }
        }
        break;

    case WM_LBUTTONDOWN:
        if (HIWORD(lParam) <= 50)
        {
            if (LOWORD(lParam) >= b1TxtRect.left && LOWORD(lParam) <= b1TxtRect.right)
            {
                if (name_set)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                    break;
                }
                if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                if (DialogBox(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &DlgProc) == IDOK)name_set = true;
                break;
            }
            if (LOWORD(lParam) >= b2TxtRect.left && LOWORD(lParam) <= b2TxtRect.right)
            {
                mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                if (sound)
                {
                    sound = false;
                    PlaySound(NULL, NULL, NULL);
                    break;
                }
                else
                {
                    sound = true;
                    PlaySound(snd_file, NULL, SND_ASYNC | SND_LOOP);
                    break;
                }
            }
            if (LOWORD(lParam) >= b3TxtRect.left && LOWORD(lParam) <= b3TxtRect.right)
            {
                if (!show_help)
                {
                    show_help = true;
                    pause = true;
                    ShowHelp();
                    break;
                }
                else
                {
                    show_help = false;
                    pause = false;
                    break;
                }
            }
        }
        else
        {
            if (Warrior)
            {
                if (Warrior->current_action == actions::block)break;
                if (sound)mciSendString(L"play .\\res\\snd\\shoot.wav", NULL, NULL, NULL);
                vGoodShots.push_back(dll::ShotFactory(types::hero_shot, Warrior->ex, Warrior->y + 40.0f,
                    (float)(LOWORD(lParam)), (float)(HIWORD(lParam))));
            }
        }
        break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)(FALSE);
}

void CreateResources()
{
    int wx = static_cast<int>(static_cast<int>(GetSystemMetrics(SM_CXSCREEN) / 2) - static_cast<int>(scr_width / 2));
    if (GetSystemMetrics(SM_CXSCREEN) < wx + scr_width || GetSystemMetrics(SM_CYSCREEN) < scr_height + 50)ErrExit(eWindow);

    mainIcon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 128, 128, LR_LOADFROMFILE));
    if (!mainIcon)ErrExit(eIcon);

    mainCur = LoadCursorFromFileW(L".\\res\\main.ani");
    outCur = LoadCursorFromFileW(L".\\res\\out.ani");
    if (!mainCur || !outCur)ErrExit(eCursor);

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &WinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(100, 100, 100));
    bWin.hCursor = mainCur;
    bWin.hIcon = mainIcon;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"ПРИКАЗНИ ЧУДОВИЩА !", WS_CAPTION | WS_SYSMENU, wx, 50, (int)(scr_width),
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else
    {
        ShowWindow(bHwnd, SW_SHOWDEFAULT);

        HRESULT hr = S_OK;
        hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
        if (hr != S_OK)
        {
            LogError(L"Error creating iFactory");
            ErrExit(eD2D);
        }

        if (iFactory && bHwnd)
        {
            hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
                D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
            if (hr != S_OK)
            {
                LogError(L"Error creating Draw");
                ErrExit(eD2D);
            }
        }

        D2D1_GRADIENT_STOP gStops[2] = { 0 };
        ID2D1GradientStopCollection* gCollection = nullptr;
        
        gStops[0].position = 0;
        gStops[0].color = D2D1::ColorF(D2D1::ColorF::DodgerBlue);
        gStops[1].position = 1.0f;
        gStops[1].color = D2D1::ColorF(D2D1::ColorF::DarkBlue);

        if (Draw)
        {
            hr = Draw->CreateGradientStopCollection(gStops, 2, &gCollection);
            if (hr != S_OK)
            {
                LogError(L"Error creating GradientStopCollection");
                ErrExit(eD2D);
            }

            if (gCollection)
            {
                hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2, 25.0f),
                    D2D1::Point2F(0, 0), scr_width / 2, 25.0f), gCollection, &butBckBr);
                if (hr != S_OK)
                {
                    LogError(L"Error creating RadialGradientBrush - background");
                    ErrExit(eD2D);
                }
                gCollection->Release();
            }

            hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &TxtBr);
            if (hr != S_OK)
            {
                LogError(L"Error creating TextBrushes");
                ErrExit(eD2D);
            }
            hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &HglTxtBr);
            if (hr != S_OK)
            {
                LogError(L"Error creating TextBrushes");
                ErrExit(eD2D);
            }
            hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DimGray), &InactTxtBr);
            if (hr != S_OK)
            {
                LogError(L"Error creating TextBrushes");
                ErrExit(eD2D);
            }

            hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &LifeBr);
            if (hr != S_OK)
            {
                LogError(L"Error creating LifeBrush");
                ErrExit(eD2D);
            }
            hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Yellow), &HurtBr);
            if (hr != S_OK)
            {
                LogError(L"Error creating HurtBrush");
                ErrExit(eD2D);
            }
            hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &CritBr);
            if (hr != S_OK)
            {
                LogError(L"Error creating CritBrush");
                ErrExit(eD2D);
            }
        }

        hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), 
            reinterpret_cast<IUnknown**>(&iWriteFactory));
        if (hr != S_OK)
        {
            LogError(L"Error creating iWriteFactory");
            ErrExit(eD2D);
        }

        if (iWriteFactory)
        {
            iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 24, L"", &nrmText);
            if (hr != S_OK)
            {
                LogError(L"Error creating nrmText format");
                ErrExit(eD2D);
            }

            iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_EXTRA_BOLD, DWRITE_FONT_STYLE_OBLIQUE,
                DWRITE_FONT_STRETCH_NORMAL, 64, L"", &bigText);
            if (hr != S_OK)
            {
                LogError(L"Error creating bigText format");
                ErrExit(eD2D);
            }
        }

        bmpField = Load(L".\\res\\img\\field.png", Draw);
        if (!bmpField)
        {
            LogError(L"Error loading field bitmap");
            ErrExit(eD2D);
        }
        bmpSky = Load(L".\\res\\img\\sky.png", Draw);
        if (!bmpSky)
        {
            LogError(L"Error loading sky bitmap");
            ErrExit(eD2D);
        }
        
        bmpHero = Load(L".\\res\\img\\hero\\hero.png", Draw);
        if (!bmpHero)
        {
            LogError(L"Error loading hero bitmap");
            ErrExit(eD2D);
        }
        bmpHeroShot = Load(L".\\res\\img\\hero\\shot.png", Draw);
        if (!bmpHeroShot)
        {
            LogError(L"Error loading hero shot bitmap");
            ErrExit(eD2D);
        }
        bmpHeroShield = Load(L".\\res\\img\\hero\\shield.png", Draw);
        if (!bmpHeroShield)
        {
            LogError(L"Error loading hero shield bitmap");
            ErrExit(eD2D);
        }

        bmpEvilShot = Load(L".\\res\\img\\evil\\shot.png", Draw);
        if (!bmpEvilShot)
        {
            LogError(L"Error loading evil shot bitmap");
            ErrExit(eD2D);
        }
        bmpEvilShield = Load(L".\\res\\img\\evil\\shield.png", Draw);
        if (!bmpEvilShield)
        {
            LogError(L"Error loading evil shield bitmap");
            ErrExit(eD2D);
        }

        bmpCentaur1 = Load(L".\\res\\img\\evil\\centaur1.png", Draw);
        if (!bmpCentaur1)
        {
            LogError(L"Error loading centaur1 bitmap");
            ErrExit(eD2D);
        }
        bmpCentaur2 = Load(L".\\res\\img\\evil\\centaur2.png", Draw);
        if (!bmpCentaur2)
        {
            LogError(L"Error loading centaur2 bitmap");
            ErrExit(eD2D);
        }
        bmpCrock = Load(L".\\res\\img\\evil\\crock.png", Draw);
        if (!bmpCrock)
        {
            LogError(L"Error loading crock bitmap");
            ErrExit(eD2D);
        }
        bmpGargoyle1 = Load(L".\\res\\img\\evil\\gargoyle1.png", Draw);
        if (!bmpGargoyle1)
        {
            LogError(L"Error loading Gargoyle1 bitmap");
            ErrExit(eD2D);
        }
        bmpGargoyle2 = Load(L".\\res\\img\\evil\\gargoyle2.png", Draw);
        if (!bmpGargoyle2)
        {
            LogError(L"Error loading Gargoyle2 bitmap");
            ErrExit(eD2D);
        }
        bmpOctopus1 = Load(L".\\res\\img\\evil\\octopus1.png", Draw);
        if (!bmpOctopus1)
        {
            LogError(L"Error loading Octopus1 bitmap");
            ErrExit(eD2D);
        }
        bmpOctopus2 = Load(L".\\res\\img\\evil\\octopus2.png", Draw);
        if (!bmpOctopus2)
        {
            LogError(L"Error loading Octopus2 bitmap");
            ErrExit(eD2D);
        }
        bmpTree = Load(L".\\res\\img\\evil\\Tree.png", Draw);
        if (!bmpTree)
        {
            LogError(L"Error loading Walking Tree bitmap");
            ErrExit(eD2D);
        }
        bmpRip = Load(L".\\res\\img\\Rip.png", Draw);
        if (!bmpRip)
        {
            LogError(L"Error loading Rip bitmap");
            ErrExit(eD2D);
        }
    }

    if (Draw && bigText && TxtBr)
    {
        mciSendString(L"play .\\res\\snd\\intro.wav", NULL, NULL, NULL);

        D2D1_RECT_F left = { -100.0f, 100.0f, 500.0f, scr_height / 2 };
        D2D1_RECT_F right = { scr_width, scr_height / 2 + 100.0f, scr_width + 350.0f, scr_height};

        bool left_ok = false;
        bool right_ok = false;

        while (!left_ok || !right_ok)
        {
            if (!left_ok)
            {
                left.left += 3.0f;
                left.right += 3.0f;
                if (left.right >= scr_width - 100.0f)left_ok = true;
            }
            if (!right_ok)
            {
                right.left -= 3.0f;
                right.right -= 3.0f;
                if (right.left <= scr_width / 2 - 100.0f)right_ok = true;
            }

            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::SandyBrown));
            Draw->DrawTextW(L"ВОЙНАТА НА ФАНТАСМАГОРИИТЕ !", 29, bigText, left, TxtBr);
            Draw->DrawTextW(L"dev. Daniel !", 14, bigText, right, TxtBr);
            Draw->EndDraw();
        }
        Sleep(3500);
    }
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);

    CreateResources();

    PlaySound(snd_file, NULL, SND_ASYNC | SND_LOOP);

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessage(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::SandyBrown));
            Draw->DrawTextW(L"ПАУЗА", 6, bigText, D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, 
                scr_width, scr_height), TxtBr);
            Draw->EndDraw();
            continue;
        }
        ////////////////////////////////////////////////////////

        if (!vGoodShots.empty())
        {
            for (std::vector<dll::Shot>::iterator good = vGoodShots.begin(); good < vGoodShots.end(); good++)
            {
                if (!(*good)->Move())
                {
                    (*good)->Release();
                    vGoodShots.erase(good);
                    break;
                }
            }
        }

        if (Warrior && !vEvilShots.empty())
        {
            for (std::vector<dll::Shot>::iterator shot = vEvilShots.begin(); shot < vEvilShots.end(); shot++)
            {
                if (!(Warrior->x >= (*shot)->ex || Warrior->ex <= (*shot)->x ||
                    Warrior->y >= (*shot)->ey || Warrior->ey <= (*shot)->y))
                {
                    if (Warrior->current_action == actions::block)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\shield.wav", NULL, NULL, NULL);
                        (*shot)->Release();
                        vEvilShots.erase(shot);
                        break;
                    }
                    if (sound)mciSendString(L"play .\\res\\snd\\hhurt.wav", NULL, NULL, NULL);
                    if (Evil) Warrior->lifes -= Evil->strenght;
                    (*shot)->Release();
                    vEvilShots.erase(shot);
                    if (Warrior->lifes <= 0)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\hkilled.wav", NULL, NULL, NULL);
                        hero_killed = true;
                        hero_killed_x = Warrior->x;
                        Warrior->Release();
                        Warrior = nullptr;
                    }
                    break;
                }
            }
        }

        //EVIL************************************
        if (!Evil && rand() % 150 == 66)
            Evil = reinterpret_cast<dll::Creature>(dll::CreatureFactory(static_cast<types>(rand() % 9 + 1), scr_width - 150.0f));
           
        if (Evil)
        {
            AI_INPUT situation = { 0 };
            if (Warrior)
            {
                situation.enemy_ex = Warrior->ex;
                situation.enemy_lifes = Warrior->lifes;
                if (!vGoodShots.empty())situation.now_shooting = true;
            }
            switch (Evil->AINextMove(situation))
            {
            case actions::move:
                Evil->Move();
                break;

            case actions::shoot:
                if (Evil->Shoot())
                {
                    vEvilShots.push_back(dll::ShotFactory(types::evil_shot, Evil->x, Evil->y + ((Evil->ey - Evil->y) / 2)));
                    if (sound)mciSendString(L"play .\\res\\snd\\shoot.wav", NULL, NULL, NULL);
                }
                break;

            case actions::block:
                if (Evil->Block())
                {
                    if (!damage_area_set)
                    {
                        damage_area_set = true;
                        switch (rand() % 2)
                        {
                        case 0:
                            evil_up_shield = false;
                            damage_area = { Evil->x, Evil->y, Evil->ex, Evil->y + 50.0f };
                            break;

                        case 1:
                            evil_up_shield = true;
                            damage_area = { Evil->x, Evil->y + 20.0f, Evil->ex, Evil->ey };
                            break;
                        }
                    }
                }
                else damage_area_set = false;
            }
        }

        if (!vEvilShots.empty())
        {
            for (std::vector<dll::Shot>::iterator shot = vEvilShots.begin(); shot < vEvilShots.end(); shot++)
            {
                if (!(*shot)->Move())
                {
                    (*shot)->Release();
                    vEvilShots.erase(shot);
                    break;
                }
            }
        }

        if (Evil && !vGoodShots.empty())
        {
            for (std::vector<dll::Shot>::iterator shot = vGoodShots.begin(); shot < vGoodShots.end(); shot++)
            {
                if (!((*shot)->x >= Evil->ex || (*shot)->ex <= Evil->x
                    || (*shot)->y >= Evil->ey || (*shot)->ey <= Evil->y))
                {
                    
                    if (Evil->current_action == actions::block)
                    {
                        if (!((*shot)->x >= damage_area.right || (*shot)->ex <= damage_area.left
                            || (*shot)->y >= damage_area.bottom || (*shot)->ey <= damage_area.top))
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\ehurt.wav", NULL, NULL, NULL);
                            (*shot)->Release();
                            vGoodShots.erase(shot);
                            score += 5;
                            Evil->lifes -= 5;
                            if (Evil->lifes <= 0)
                            {
                                evil_killed = true;
                                evil_killed_delay = 500;
                                evil_killed_x = Evil->x;
                                score += 50;
                                if (sound)mciSendString(L"play .\\res\\snd\\ekilled.wav", NULL, NULL, NULL);
                                Evil->Release();
                                Evil = nullptr;
                            }
                            break;
                        }
                        else
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\shield.wav", NULL, NULL, NULL);
                            (*shot)->Release();
                            vGoodShots.erase(shot);
                            break;
                        }
                    }
                    else
                    {
                        (*shot)->Release();
                        vGoodShots.erase(shot);
                        if (sound)mciSendString(L"play .\\res\\snd\\ehurt.wav", NULL, NULL, NULL);
                        score += 10;
                        Evil->lifes -= 10;
                        if (Evil->lifes <= 0)
                        {
                            evil_killed = true;
                            evil_killed_delay = 500;
                            evil_killed_x = Evil->x;
                            score += 50;
                            Evil->Release();
                            Evil = nullptr;
                        }
                        break;
                    }
                }

            }
        }

        if (Evil && Warrior)
        {
            if (!(Warrior->x >= Evil->ex || Warrior->ex <= Evil->x || Warrior->y >= Evil->ey || Warrior->ey <= Evil->y))
            {
                if (sound)mciSendString(L"play .\\res\\snd\\hhurt.wav", NULL, NULL, NULL);
                Evil->x += 50.0f;
                Evil->SetEdges();
                Warrior->lifes -= Evil->strenght * 2;
                if (Warrior->lifes <= 0)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\hkilled.wav", NULL, NULL, NULL);
                    hero_killed = true;
                    hero_killed_x = Warrior->x;
                    Warrior->Release();
                    Warrior = nullptr;
                }
            }
        }

        //DRAW THINGS ****************************

        if (Draw)
        {
            Draw->BeginDraw();
            if (nrmText && bigText && butBckBr && TxtBr && InactTxtBr && HglTxtBr)
            {
                Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), butBckBr);
                if (name_set)
                    Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, InactTxtBr);
                else if(b1Hglt)
                    Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, HglTxtBr);
                else
                    Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmText, b1TxtRect, TxtBr);
                if (b2Hglt)
                    Draw->DrawTextW(L"ЗВУЦИ ON /OFF", 14, nrmText, b2TxtRect, HglTxtBr);
                else
                    Draw->DrawTextW(L"ЗВУЦИ ON /OFF", 14, nrmText, b2TxtRect, TxtBr);
                if (b3Hglt)
                    Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, HglTxtBr);
                else
                    Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmText, b3TxtRect, TxtBr);
            }
            Draw->DrawBitmap(bmpSky, D2D1::RectF(0, 50.0f, scr_width, scr_height - 95.0f));
            Draw->DrawBitmap(bmpField, D2D1::RectF(0, scr_height - 100.0f, scr_width, scr_height));
            
            
            ///////////////////////////////////////////////////////////////

            //WARRIOR ***************************

            if (Warrior)
            {
                Draw->DrawBitmap(bmpHero, D2D1::RectF(Warrior->x, Warrior->y, Warrior->ex, Warrior->ey));
                if (LifeBr && HurtBr && CritBr)
                {
                    if (Warrior->lifes > 150)
                        Draw->DrawLine(D2D1::Point2F(Warrior->x, Warrior->ey + 5.0f),
                            D2D1::Point2F(Warrior->x + static_cast<float>(Warrior->lifes / 4),
                                Warrior->ey + 5.0f), LifeBr, 10.0f);
                    else if (Warrior->lifes > 60)
                        Draw->DrawLine(D2D1::Point2F(Warrior->x, Warrior->ey + 5.0f),
                            D2D1::Point2F(Warrior->x + static_cast<float>(Warrior->lifes / 4),  
                                Warrior->ey + 5.0f), HurtBr, 10.0f);
                    else
                        Draw->DrawLine(D2D1::Point2F(Warrior->x, Warrior->ey + 5.0f),
                            D2D1::Point2F(Warrior->x + static_cast<float>(Warrior->lifes / 4), 
                                Warrior->ey + 5.0f), CritBr, 10.0f);
                }
                if (Warrior->current_action == actions::block)
                {
                    Warrior->Block();
                    Draw->DrawBitmap(bmpHeroShield, D2D1::RectF(Warrior->ex, Warrior->y, Warrior->ex + 55.0f, Warrior->ey));
                }
            }

            if (!vGoodShots.empty())
            {
                for (std::vector<dll::Shot>::iterator good = vGoodShots.begin(); good < vGoodShots.end(); good++)
                {
                    Draw->DrawBitmap(bmpHeroShot, D2D1::RectF((*good)->x, (*good)->y, (*good)->ex, (*good)->ey));
                }
            }

            //EVIL *****************************

            if (Evil)
            {
                switch (Evil->GetType())
                {
                case types::centaur1:
                    Draw->DrawBitmap(bmpCentaur1, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                case types::centaur2:
                    Draw->DrawBitmap(bmpCentaur2, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                case types::croc:
                    Draw->DrawBitmap(bmpCrock, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                case types::gargoyle1:
                    Draw->DrawBitmap(bmpGargoyle1, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                case types::gargoyle2:
                    Draw->DrawBitmap(bmpGargoyle2, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                case types::octopus1:
                    Draw->DrawBitmap(bmpOctopus1, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                case types::octopus2:
                    Draw->DrawBitmap(bmpOctopus2, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                case types::walking_tree:
                    Draw->DrawBitmap(bmpTree, D2D1::RectF(Evil->x, Evil->y, Evil->ex, Evil->ey));
                    break;

                }

                if (LifeBr && HurtBr && CritBr)
                {
                    if (Evil->lifes > 120)
                        Draw->DrawLine(D2D1::Point2F(Evil->x, Evil->ey + 5.0f),
                            D2D1::Point2F(Evil->x + static_cast<float>(Evil->lifes / 4), 
                                Evil->ey + 5.0f), LifeBr, 10.0f);
                    else if (Evil->lifes > 60)
                        Draw->DrawLine(D2D1::Point2F(Evil->x, Evil->ey + 5.0f),
                            D2D1::Point2F(Evil->x + static_cast<float>(Evil->lifes / 4), 
                                Evil->ey + 5.0f), HurtBr, 10.0f);
                    else
                        Draw->DrawLine(D2D1::Point2F(Evil->x, Evil->ey + 5.0f),
                            D2D1::Point2F(Evil->x + static_cast<float>(Evil->lifes / 4), 
                                Evil->ey + 5.0f), CritBr, 10.0f);
                }
                if (Evil->current_action == actions::block)
                {
                    if(evil_up_shield)
                        Draw->DrawBitmap(bmpEvilShield, D2D1::RectF(Evil->x, Evil->y - 80.0f, Evil->x + 80.0f, Evil->y));
                    else
                        Draw->DrawBitmap(bmpEvilShield, D2D1::RectF(Evil->x - 80.0f, Evil->y + 40.0f, Evil->x, Evil->y + 120.0f));
                    Evil->Block();
                }
            
            }

            if (!vEvilShots.empty())
            {
                for (std::vector<dll::Shot>::iterator shot = vEvilShots.begin(); shot < vEvilShots.end(); shot++)
                {
                    Draw->DrawBitmap(bmpEvilShot, D2D1::RectF((*shot)->x, (*shot)->y, (*shot)->ex, (*shot)->ey));
                }
            }

            //STATUS *****************************

            wchar_t status[150] = L"ВОЙНИК: ";
            wchar_t add[5] = L"\0";
            int txt_size = 0;

            wcscat_s(status, current_player);
            
            wcscat_s(status, L", ВРЕМЕ: ");
            if (mins < 10)wcscat_s(status, L"0");
            wsprintf(add, L"%d", mins);
            wcscat_s(status, add);

            wcscat_s(status, L" : ");
            if (secs - mins * 60 < 10)wcscat_s(status, L"0");
            wsprintf(add, L"%d", secs - mins * 60);
            wcscat_s(status, add);

            wcscat_s(status, L", РЕЗУЛТАТ: ");
            wsprintf(add, L"%d", score);
            wcscat_s(status, add);

            for (int i = 0; i < 150; i++)
            {
                if (status[i] != '\0')txt_size++;
                else break;
            }

            if (nrmText && HglTxtBr)
                Draw->DrawTextW(status, txt_size, nrmText, D2D1::RectF(30.0f, scr_height - 80.0f, scr_width, scr_height), HglTxtBr);

            //KILLS ******************************
            if (evil_killed)
            {
                Draw->DrawBitmap(bmpRip, D2D1::RectF(evil_killed_x, scr_height - 217.0f, evil_killed_x + 100.0f, scr_height - 100.0f));
                evil_killed_delay--;
                if (evil_killed_delay <= 0)evil_killed = false;
            }

            if (hero_killed)
            {
                Draw->DrawBitmap(bmpRip, D2D1::RectF(hero_killed_x, scr_height - 217.0f, hero_killed_x + 100.0f, 
                    scr_height - 100.0f));
                hero_killed_delay--;
                if (hero_killed_delay <= 0)
                {
                    Draw->EndDraw();
                    GameOver();
                }
            }
            
            ///////////////////////////////////////////////////
            Draw->EndDraw();
        }

    }

    ReleaseResources();
    std::remove(tmp_file);
    return (int) bMsg.wParam;
}