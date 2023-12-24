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
#include "../imgui/imgui_impl_dx9.h"
#include "../imgui/imgui_impl_win32.h"


HANDLE process = NULL;
DWORD id = 0;


class Addresses {
public:
    long long int money;
    long long int stars;
    long long int restockCount;
    long long int speed;
    long long int fireRate;
    long long int multishot;
    long long int homing;
    long long int wealth;
    long long int wallPunch;
    long long int heal;
    long long int maxHealth;
    long long int freezing;
    long long int piercing;
    long long int splashDamage;
    long long int health;
};
long long int money;
long long int stars;
long long int speed;
long long int fireRate;
long long int multishot;
long long int homing;
long long int wealth;
long long int wallPunch;
long long int maxHealth;
long long int freezing;
long long int piercing;
long long int splashDamage;
long long int health;
long long int sleepTime = 150;

bool Write(long long int address, long long int var) {
    return WriteProcessMemory(process, (LPVOID)(address), &var, sizeof(long long int), 0);
}




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
    long long int offsets[6] = { 0x03453100, 0x348, 0x1C0, 0x10, 0x68, 0x28 };
    for (int i = 0; i < 6; i++) {
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
long long int Read(long long int address) {
    long long int var = NULL;
    process = GetProcess();
    ReadProcessMemory(process, (LPCVOID)(address), &var, sizeof(long long int), 0);
    return var;
}

void UpdateValues(Addresses addresses) {
    while (true) {
        money = Read(addresses.money);
        stars = Read(addresses.stars);
        speed = Read(addresses.speed) + 1;
        fireRate = Read(addresses.fireRate) + 1;
        multishot = Read(addresses.multishot) + 1;
        homing = Read(addresses.homing) + 1;
        wealth = Read(addresses.wealth) + 1;
        wallPunch = Read(addresses.wallPunch) + 1;
        maxHealth = Read(addresses.maxHealth) + 1;
        freezing = Read(addresses.freezing) + 1;
        piercing = Read(addresses.piercing) + 1;
        splashDamage = Read(addresses.splashDamage) + 1;
        health = Read(addresses.health);
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
    }
}
#define HOTKEY_ID 1
int __stdcall wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    RegisterHotKey(NULL, HOTKEY_ID, MOD_ALT, VK_F4);
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));



    Addresses addresses;

    process = GetProcess();
    std::printf("The process id: %d \n", id);
    long long int offset = 0;
    const auto base = GetModuleAddress("windowkill-vulkan.exe");
    offset = getAddress(base);

    if (!process){
        MessageBox(NULL, "Please start the game first!", "Game not found!", 0);
        return 0;
    }
    addresses.money = offset + 0x260;
    addresses.stars = offset + 0x278;
    addresses.restockCount = offset + 0x2A8;
    addresses.speed = offset + 0x2D8;
    addresses.fireRate = offset + 0x2F0;
    addresses.multishot = offset + 0x308;
    addresses.homing = offset + 0x320;
    addresses.wealth = offset + 0x338;
    addresses.wallPunch = offset + 0x350;
    addresses.heal = offset + 0x368;
    addresses.maxHealth = offset + 0x380;
    addresses.freezing = offset + 0x398;
    addresses.piercing = offset + 0x3B0;
    addresses.splashDamage = offset + 0x3C8;
    addresses.health = offset + 0x470;


    std::thread updateThread(UpdateValues, addresses);

    bool lockMoney = false;
    bool lockStars = false;
    bool lockHealth = false;
    long long int startMoney = Read(addresses.money);
    long long int startStars = Read(addresses.stars);
    long long int startHealth = Read(addresses.health);



    cheat::CreateHWindow("WindowKill", "Cheat Menu Class");
    cheat::CreateDevice();
    cheat::CreateImGui();

    while (cheat::exit) {
        PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT || msg.message == WM_CLOSE) {
            cheat::exit = false;
        }


        cheat::BeginRender();
        ImGui::SetNextWindowPos({ 0, 0 });
        ImGui::SetNextWindowSize({ cheat::WIDTH * 1.0f, cheat::HEIGHT * 1.0f});
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        if (ImGui::Begin("WindowKill Hack", &cheat::exit, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove)) {
            cheat::x = ImGui::GetWindowWidth();
            cheat::y = ImGui::GetWindowHeight();
            ImGui::SetNextItemWidth(100);
            ImGui::InputInt("Update time (milliseconds)", &sleepTime);
            ImGui::PushItemWidth(187);
            if (ImGui::InputInt("Money", &money)) Write(addresses.money, money);
            ImGui::SameLine();
            if (ImGui::Checkbox("Lock##money", &lockMoney)) {
                startMoney = money;
            }
            if (ImGui::InputInt("Stars", &stars)) Write(addresses.stars, stars);
            ImGui::SameLine();
            if (ImGui::Checkbox("Lock##stars", &lockStars)) {
                startStars = stars;
            }
            if (ImGui::InputInt("Speed", &speed)) Write(addresses.speed, speed - 1);
            if (ImGui::InputInt("Fire Rate", &fireRate)) Write(addresses.fireRate, fireRate - 1);
            if (ImGui::InputInt("Multi Shot", &multishot)) Write(addresses.multishot, multishot - 1);
            if (ImGui::InputInt("Homing", &homing)) Write(addresses.homing, homing - 1);
            if (ImGui::InputInt("Wealth", &wealth)) Write(addresses.wealth, wealth - 1);
            if (ImGui::InputInt("Wall Punch", &wallPunch)) Write(addresses.wallPunch, wallPunch - 1);
            if (ImGui::InputInt("Max Health", &maxHealth)) Write(addresses.maxHealth, maxHealth - 1);
            if (ImGui::InputInt("Freezing", &freezing)) Write(addresses.freezing, freezing - 1);
            if (ImGui::InputInt("Piercing", &piercing)) Write(addresses.piercing, piercing - 1);
            if (ImGui::InputInt("Splash Damage", &splashDamage)) Write(addresses.splashDamage, splashDamage - 1);
            if (ImGui::InputInt("Health", &health)) Write(addresses.health, health);
            ImGui::SameLine();
            if (ImGui::Checkbox("Lock##health", &lockHealth)) {
                startHealth = health;
            }

            if (lockMoney) {
                money = startMoney;
                Write(addresses.money, startMoney);
            }
            if (lockStars) {
                stars = startStars;
                Write(addresses.stars, startStars);
            }
            if (lockHealth) {
                health = startHealth;
                Write(addresses.health, startHealth);
            }

            ImGui::PopItemWidth();
            //ImGui::Text("Width: %f\nHeight: %f", ImGui::GetWindowWidth(), ImGui::GetWindowHeight());
            ImGui::Text("Application average %.3f ms/frame (%.0f FPS)", 1000.0f / io.Framerate, io.Framerate);
        }
        ImGui::End();
        cheat::EndRender();
        
        
        HWND foreground = GetForegroundWindow();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        if (!process || (GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_F4) & 0x8000) && foreground == cheat::window) {
            cheat::exit = false;
        }

    }
    cheat::DestroyImGui();
    cheat::DestroyDevice();
    cheat::DestroyHWindow();
    return 0;
}