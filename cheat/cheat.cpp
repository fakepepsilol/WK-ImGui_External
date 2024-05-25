#include<Windows.h>
#include<TlHelp32.h>
#include<iostream>
#include<tchar.h>
#include<vector>
#include<stdlib.h>
#include<thread>
#include<random>
#include<sstream>
#include "gui.h"
#include "../imgui/imgui.h"

HANDLE process = NULL;
DWORD id = 0;


uintptr_t GetModuleAddress(const char* moduleName) {
    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);
    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, id);
    uintptr_t result = 0;
    while (Module32Next(snapShot, &entry)) {
        if (!strcmp(moduleName, entry.szModule)) {
            result = reinterpret_cast<uintptr_t>(entry.modBaseAddr);
            break;
        }
    }
    if (snapShot) {
        CloseHandle(snapShot);
    }
    return result;
}
long long int getAddress(long long int base) {
    long long int offsets[9] = { 0x034541E0, 0x310, 0x60, 0x220, 0x348, 0x1C0, 0x10, 0x68, 0x28};
    for (int i = 0; i < 9; i++) {
        ReadProcessMemory(process, (LPCVOID)(base + offsets[i]), &base, sizeof(long long int), 0);
    }
    return base;
}

HANDLE GetProcess() {
    const char* processName = "windowkill-vulkan.exe";

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);


    const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    while (Process32Next(snapShot, &entry)) {
        if (!strcmp(processName, entry.szExeFile)) {
            id = entry.th32ProcessID;
            CloseHandle(snapShot);
            return OpenProcess(PROCESS_ALL_ACCESS, FALSE, id);
            break;
        }
    }
    return NULL;
}
bool cheat::Write(long long int address, long long int var) {
    return WriteProcessMemory(process, (LPVOID)(address), &var, sizeof(long long int), 0);
}
long long int cheat::Read(long long int address) {
    long long int var = NULL;
    process = GetProcess();
    ReadProcessMemory(process, (LPCVOID)(address), &var, sizeof(long long int), 0);
    return var;
}

void cheat::UpdateValues(cheat::Addresses* addresses) {
    while (true) {
        money = cheat::Read(addresses->money);
        stars = cheat::Read(addresses->stars);
        speed = cheat::Read(addresses->speed) + 1;
        fireRate = cheat::Read(addresses->fireRate) + 1;
        multishot = cheat::Read(addresses->multishot) + 1;
        homing = cheat::Read(addresses->homing) + 1;
        wealth = cheat::Read(addresses->wealth) + 1;
        wallPunch = cheat::Read(addresses->wallPunch) + 1;
        freezing = cheat::Read(addresses->freezing) + 1;
        piercing = cheat::Read(addresses->piercing) + 1;
        splashDamage = cheat::Read(addresses->splashDamage) + 1;
        maxHealth = cheat::Read(addresses->maxHealth);
        health = cheat::Read(addresses->health);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}
#define HOTKEY_ID 1
int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));

    cheat::lockMoney = false;
    cheat::lockStars = false;
    cheat::lockHealth = false;

    cheat::startMoney = cheat::Read(cheat::addresses.money);
    cheat::startStars = cheat::Read(cheat::addresses.stars);
    cheat::startHealth = cheat::Read(cheat::addresses.health);


    process = GetProcess();
    long long int offset = 0;
    uintptr_t base = GetModuleAddress("windowkill-vulkan.exe");
    offset = getAddress(base);
    if (!process) {
        MessageBox(NULL, "Please start the game first!", "Proccess not found!", 0);
        return 0;
    }
    while (cheat::addresses.health == 0x488 || cheat::addresses.health == NULL) {
        offset = 0;
        base = GetModuleAddress("windowkill-vulkan.exe");
        offset = getAddress(base);
        cheat::addresses.money = offset + 0x278;
        cheat::addresses.stars = offset + 0x290; ///wqaaaaaah
        cheat::addresses.restockCount = offset + 0x2C0;
        cheat::addresses.speed = offset + 0x2F0;
        cheat::addresses.fireRate = offset + 0x308;
        cheat::addresses.multishot = offset + 0x320;
        cheat::addresses.homing = offset + 0x338;
        cheat::addresses.wealth = offset + 0x350;
        cheat::addresses.wallPunch = offset + 0x368;
        cheat::addresses.heal = offset + 0x398;
        cheat::addresses.freezing = offset + 0x3B0;
        cheat::addresses.piercing = offset + 0x3C8;
        cheat::addresses.splashDamage = offset + 0x3E0;
        cheat::addresses.maxHealth = offset + 0x470;
        cheat::addresses.health = offset + 0x488;
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }

    std::thread updateThread(cheat::UpdateValues, &cheat::addresses);




    cheat::CreateHWindow("Cheat Menu");
    cheat::CreateDevice();
    cheat::CreateImGui();

    while (cheat::isRunning && process)
    {
        cheat::BeginRender();
        cheat::Render();
        cheat::EndRender();

        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        HWND foreground = GetForegroundWindow();
        if (!process || (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_F4) & 0x8000) && foreground == cheat::window) {
            cheat::exit = false;
        }
    }

    cheat::DestroyImGui();
    cheat::DestroyDevice();
    cheat::DestroyHWindow();

    return EXIT_SUCCESS;



}
