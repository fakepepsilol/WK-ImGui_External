#pragma once

#include<d3d9.h>
namespace cheat {
	inline int WIDTH = 315;
	inline int HEIGHT = 400;
	inline int x = WIDTH, y = HEIGHT;
	inline bool exit = true;


	inline HWND window = nullptr;
	inline WNDCLASSEXA windowClass = {};

	inline POINTS position = {};
	inline PDIRECT3D9 d3d = nullptr;
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline D3DPRESENT_PARAMETERS presentParameters = {};

	void CreateHWindow(const char* windowName, const char* className) noexcept;
	void DestroyHWindow() noexcept;

	bool CreateDevice() noexcept;
	void ResetDevice() noexcept;
	void DestroyDevice() noexcept;

	void CreateImGui() noexcept;
	void DestroyImGui() noexcept;
	
	void BeginRender() noexcept;
	void EndRender() noexcept;
	void Render() noexcept;
	
	
	#ifndef ADDRESSES_H
	#define ADDRESSES_H
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
	#endif


	long long int Read(long long int address);
	bool Write(long long int address, long long int var);
}