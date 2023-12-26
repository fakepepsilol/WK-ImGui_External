#include<string>
#include"gui.h"
#include"../imgui/imgui.h"
#include"../imgui/imgui_impl_dx9.h"
#include"../imgui/imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter
);
long __stdcall WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParameter,
	LPARAM longParameter)
{
	if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
		return true;

	switch (message)
	{
	case WM_SIZE: {
		if (cheat::device && wideParameter != SIZE_MINIMIZED)
		{
			cheat::presentParameters.BackBufferWidth = LOWORD(longParameter);
			cheat::presentParameters.BackBufferHeight = HIWORD(longParameter);
			cheat::ResetDevice();
		}
	}return 0;

	case WM_SYSCOMMAND: {
		if ((wideParameter & 0xfff0) == SC_KEYMENU)
			return 0;
	}break;

	case WM_DESTROY: {
		PostQuitMessage(0);
	}return 0;

	case WM_LBUTTONDOWN: {
		cheat::position = MAKEPOINTS(longParameter);
	}return 0;

	case WM_MOUSEMOVE: {
		if (wideParameter == MK_LBUTTON)
		{
			const auto points = MAKEPOINTS(longParameter);
			auto rect = ::RECT{ };

			GetWindowRect(cheat::window, &rect);

			rect.left += points.x - cheat::position.x;
			rect.top += points.y - cheat::position.y;

			if (cheat::position.x >= 0 &&
				cheat::position.x <= cheat::WIDTH &&
				cheat::position.y >= 0 && cheat::position.y <= 19)
				SetWindowPos(
					cheat::window,
					HWND_TOPMOST,
					rect.left,
					rect.top,
					0, 0,
					SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
				);
		}

	}return 0;

	}
    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	DefWindowProc(window, message, wideParameter, longParameter);
	return 1;
}

void cheat::CreateHWindow(const char* windowName) noexcept
{
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_CLASSDC;
	windowClass.lpfnWndProc = (WNDPROC)WindowProcess;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandleA(0);
	windowClass.hIcon = 0;
	windowClass.hCursor = 0;
	windowClass.hbrBackground = 0;
	windowClass.lpszMenuName = 0;
	windowClass.lpszClassName = "class001";
	windowClass.hIconSm = 0;

	RegisterClassEx(&windowClass);

	window = CreateWindowEx(
		0,
		"class001",
		windowName,
		WS_POPUP,
		100,
		100,
		WIDTH,
		HEIGHT,
		0,
		0,
		windowClass.hInstance,
		0
	);

	ShowWindow(window, SW_SHOWDEFAULT);
	UpdateWindow(window);
}

void cheat::DestroyHWindow() noexcept
{
	DestroyWindow(window);
	UnregisterClass(windowClass.lpszClassName, windowClass.hInstance);
}

bool cheat::CreateDevice() noexcept
{
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d)
		return false;

	ZeroMemory(&presentParameters, sizeof(presentParameters));

	presentParameters.Windowed = TRUE;
	presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
	presentParameters.EnableAutoDepthStencil = TRUE;
	presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
	presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (d3d->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParameters,
		&device) < 0)
		return false;

	return true;
}

void cheat::ResetDevice() noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	const auto result = device->Reset(&presentParameters);

	if (result == D3DERR_INVALIDCALL)
		IM_ASSERT(0);

	ImGui_ImplDX9_CreateDeviceObjects();
}

void cheat::DestroyDevice() noexcept
{
	if (device)
	{
		device->Release();
		device = nullptr;
	}

	if (d3d)
	{
		d3d->Release();
		d3d = nullptr;
	}
}

void cheat::CreateImGui() noexcept
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ::ImGui::GetIO();
	io.IniFilename = NULL;


	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);
}

void cheat::DestroyImGui() noexcept
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void cheat::BeginRender() noexcept
{
	MSG message;
	while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);

		if (message.message == WM_QUIT || message.message == WM_CLOSE) {
			cheat::isRunning = false;
		}
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void cheat::EndRender() noexcept
{
	ImGui::EndFrame();

	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

	device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	if (device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	const auto result = device->Present(0, 0, 0, 0);

	if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		ResetDevice();
}

void cheat::Render() noexcept
{
	ImGui::SetNextWindowPos({ 0, 0 });
	ImGui::SetNextWindowSize({ WIDTH, HEIGHT });
	ImGui::Begin(
		"Windowkill",
		&isRunning,
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoMove
	);

	ImGui::SetNextItemWidth(100);
	ImGui::InputInt("Update time (milliseconds)", &sleepTime);
	ImGui::PushItemWidth(187);
	if (ImGui::InputInt("Money", &money, 1, 100, cheat::lockMoney ? ImGuiInputTextFlags_ReadOnly : NULL)) Write(addresses.money, money);
	ImGui::SameLine();
	if (ImGui::Checkbox("Lock##money", &cheat::lockMoney)) {
		cheat::startMoney = cheat::money;
	}
	if (ImGui::InputInt("Stars", &stars, 1, 100, cheat::lockStars ? ImGuiInputTextFlags_ReadOnly : NULL)) Write(addresses.stars, stars);
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
	if (ImGui::InputInt("Freezing", &freezing)) Write(addresses.freezing, freezing - 1);
	if (ImGui::InputInt("Piercing", &piercing)) Write(addresses.piercing, piercing - 1);
	if (ImGui::InputInt("Splash Damage", &splashDamage)) Write(addresses.splashDamage, splashDamage - 1);
	if (ImGui::InputInt("Max Health", &maxHealth)) Write(addresses.maxHealth, maxHealth);
	if (ImGui::InputInt("Health", &health, 1, 100, lockHealth ? ImGuiInputTextFlags_ReadOnly : NULL)) Write(addresses.health, health);
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

	ImGui::End();
}
