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

ImVec4 hex2rgba(int hexValue, float opacity) {
	double r = ((hexValue >> 16) & 0xFF) / 255.0;
	double g = ((hexValue >> 8) & 0xFF) / 255.0;
	double b = ((hexValue) & 0xFF) / 255.0;
	return ImVec4((float)r, (float)g, (float)b, opacity);
}
void setTheme(int themeIndex, ImGuiStyle* style) {
	int textColor = 0;
	int backgroundColor = 0;
	int primaryColor = 0;
	int secondaryColor = 0;
	int accentColor = 0;
	switch (themeIndex) {
	case 0:
		textColor = 0xe5f3f3;
		backgroundColor = 0x060e0e;
		primaryColor = 0x9ad8d7;
		secondaryColor = 0x297574;
		accentColor = 0x60cdcb;
		break;
	case 1:
		textColor = 0xfce3fd;
		backgroundColor = 0x200222;
		primaryColor = 0xe680f7;
		secondaryColor = 0x971b0a;
		accentColor = 0xf28b2d;
		break;
	case 2:
		textColor = 0xecefe5;
		backgroundColor = 0x12140b;
		primaryColor = 0xc5d0a5;
		secondaryColor = 0x5e6c35;
		accentColor = 0xa2b864;
		break;
	case 3:
		textColor = 0xf9e9ed;
		backgroundColor = 0x18060a;
		primaryColor = 0xe1899d;
		secondaryColor = 0x848e24;
		accentColor = 0x96d451;
		break;
	case 4:
		textColor = 0xdafbdb;
		backgroundColor = 0x010a02;
		primaryColor = 0x7df28b;
		secondaryColor = 0x3e10a1;
		accentColor = 0xec42d9;
		break;
	case 5:
		textColor = 0xf1e3e6;
		backgroundColor = 0x1a0e0e;
		primaryColor = 0xcfa1a6;
		secondaryColor = 0x6f6738;
		accentColor = 0x98ab5b;
		break;
	case 6:
		textColor = 0xf0f1e5;
		backgroundColor = 0x111109;
		primaryColor = 0xcdcfa6;
		secondaryColor = 0x3c7050;
		accentColor = 0x65ab9d;
		break;
	case 7:
		textColor = 0xd6fed8;
		backgroundColor = 0x011803;
		primaryColor = 0x82fc8b;
		secondaryColor = 0x048e9c;
		accentColor = 0x09a1f8;
		break;
	case 8:
		textColor = 0xe9e6f2;
		backgroundColor = 0x0f0d18;
		primaryColor = 0xaca4cf;
		secondaryColor = 0x66386a;
		accentColor = 0xab60a2;
		break;
	case 9:
		textColor = 0xfcfbe4;
		backgroundColor = 0x0b0b01;
		primaryColor = 0xf4ef8a;
		secondaryColor = 0x10a187;
		accentColor = 0x4398ed;
		break;
	case 10:
		textColor = 0xe8dcf0;
		backgroundColor = 0x130a1a;
		primaryColor = 0xc29bdf;
		secondaryColor = 0x55227a;
		accentColor = 0x8f37cf;
		break;
	case 11:
		textColor = 0xf3e8f0;
		backgroundColor = 0x1a0e17;
		primaryColor = 0xd3aac6;
		secondaryColor = 0x75733c;
		accentColor = 0x83ab5c;
		break;
	case 12:
		textColor = 0xdae2f7;
		backgroundColor = 0x03060d;
		primaryColor = 0x94b2e9;
		secondaryColor = 0x6c1e92;
		accentColor = 0xd943d2;
		break;
	case 13:
		textColor = 0xf6f1ee;
		backgroundColor = 0x110c09;
		primaryColor = 0xd2b5ab;
		secondaryColor = 0x70683c;
		accentColor = 0xb2b879;
		break;
	case 14:
		textColor = 0xeee5de;
		backgroundColor = 0x100c08;
		primaryColor = 0xcfb5a0;
		secondaryColor = 0x39713a;
		accentColor = 0x66b194;
		break;
	case 15:
		textColor = 0xfee1f9;
		backgroundColor = 0x25011f;
		primaryColor = 0xfa75e0;
		secondaryColor = 0x980539;
		accentColor = 0xf83862;
		break;
	case 16:
		textColor = 0xfafddd;
		backgroundColor = 0x222402;
		primaryColor = 0xe7f67c;
		secondaryColor = 0x0a9417;
		accentColor = 0x28f187;
		break;
	}
	style->Colors[ImGuiCol_Text]			=	 hex2rgba(textColor, 1.0f);
	style->Colors[ImGuiCol_CheckMark]		=	 hex2rgba(textColor, 1.0f);
	style->Colors[ImGuiCol_WindowBg]		=	 hex2rgba(backgroundColor, 1.0f);
	style->Colors[ImGuiCol_FrameBgActive]	=	 hex2rgba(primaryColor, 0.7f);
	style->Colors[ImGuiCol_ButtonActive]	=	 hex2rgba(primaryColor, 0.7f);
	style->Colors[ImGuiCol_TitleBgActive]	=	 hex2rgba(secondaryColor, 1.0f);
	style->Colors[ImGuiCol_Button]			=	 hex2rgba(secondaryColor, 1.0f);
	style->Colors[ImGuiCol_ButtonHovered]	=	 hex2rgba(accentColor, 1.0f);
	style->Colors[ImGuiCol_FrameBgHovered]	=	 hex2rgba(accentColor, 0.3f);

	return;
}

int theme = 0;
int themeCount = 16;
void cheat::Render() noexcept
{
	ImGuiStyle* style = &ImGui::GetStyle();
	
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
	setTheme(theme, style);
	if (ImGui::Button("<-")) {
		if (theme == 0) {
			theme = themeCount;
		}
		else {
			theme--;
		}
	}
	ImGui::SameLine();
	ImGui::Text("Theme");
	ImGui::SameLine();
	if (ImGui::Button("->")) {
		if (theme == themeCount) {
			theme = 0;
		}
		else {
			theme++;
		}
	}
	ImGui::SameLine(); 
	ImGui::Text(std::to_string(theme).c_str());
	ImGui::SetNextItemWidth(100);
	ImGui::InputInt("Update interval (ms)", &sleepTime);
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
