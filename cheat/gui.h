#pragma once
#include <d3d9.h>

namespace cheat
{
	inline long long int money;
	inline long long int stars;
	inline long long int speed;
	inline long long int fireRate;
	inline long long int multishot;
	inline long long int homing;
	inline long long int wealth;
	inline long long int wallPunch;
	inline long long int maxHealth;
	inline long long int freezing;
	inline long long int piercing;
	inline long long int splashDamage;
	inline long long int health;
	inline long long int sleepTime = 150;

	inline bool lockMoney = false;
	inline bool lockStars = false;
	inline bool lockHealth = false;


	inline long long int startMoney;
	inline long long int startStars;
	inline long long int startHealth;


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
	inline Addresses addresses;
	inline bool exit = true;
	long long int Read(long long int address);
	bool Write(long long int address, long long int var);
	void UpdateValues(Addresses* addresses);


	constexpr int WIDTH = 310;
	constexpr int HEIGHT = 360;
	inline bool isRunning = true;
	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = { };
	inline POINTS position = { };
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = { };
	void CreateHWindow(const char* windowName) noexcept;
	void DestroyHWindow() noexcept;
	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;
	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;
	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
}
