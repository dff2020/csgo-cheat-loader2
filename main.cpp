// dear imgui: standalone example application for DirectX 9
// If you are new to dear imgui, see examples/README.txt and documentation at the top of imgui.cpp.

#include "includes.hpp"
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>

#include "injector/memject.c"

#define wx 500
#define wy 300

// Data
static LPDIRECT3D9              g_pD3D = NULL;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static D3DPRESENT_PARAMETERS    g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main(int, char**)
{
    FreeConsole();

    using namespace c_auth;

    c_api::c_init(c_xor("version of the program"), c_xor("program key"), c_xor("program api/encryption key"));

    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("cuh"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("nibba word"), WS_VISIBLE | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 100, 100, wx, wy, NULL, NULL, wc.hInstance, NULL);

    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    bool main_window = true;

    bool login_window = false;
    bool register_window = false;
    bool activate_window = false;

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos({ 0,0 });
        ImGui::SetNextWindowSize({ wx, wy });

        if (main_window) 
        {
            ImGui::Begin(c_xor("123!"), &main_window, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text(c_xor("Choose a form please : ")); 

            if (ImGui::Button(c_xor("Login Form"))) {
                form_defs::clear();

                login_window = true;
                register_window = false;
                activate_window = false;
            }

            ImGui::SameLine();

            if (ImGui::Button(c_xor("Register Form"))) {
                form_defs::clear();

                register_window = true;
                login_window = false;
                activate_window = false;
            }

            ImGui::SameLine();

            if (ImGui::Button(c_xor("Activation Form"))) {
                form_defs::clear();

                activate_window = true;
                login_window = false;
                register_window = false;
            }

            ImGui::End();
        }
        else 
        {
            ImGui::Begin(c_xor("1234!"), (bool*)true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove); 

            ImGui::Text(c_xor("user : %s"), c_userdata::username.c_str());
            ImGui::Text(c_xor("email : %s"), c_userdata::email.c_str());
            ImGui::Text(c_xor("expires : %s"), utils::time_to_str(c_userdata::expires).c_str());
            ImGui::Text(c_xor("rank : %i"), c_userdata::rank);

            if (ImGui::Button(c_xor("Inject"))) {
                inject_dll();
                exit(0);
            }

            ImGui::End();
        }

        if (login_window) {
            ImGui::SetNextWindowSize({ 300, 200 });

            ImGui::Begin(c_xor("Login Window"), &login_window);
            ImGui::InputTextWithHint(c_xor("##user_box"), c_xor("user"), form_defs::username, IM_ARRAYSIZE(form_defs::username));
            ImGui::InputTextWithHint(c_xor("##pass_box"), c_xor("pass"), form_defs::password, IM_ARRAYSIZE(form_defs::password), ImGuiInputTextFlags_Password);

            if (ImGui::Button(c_xor("Login")) && c_api::c_login(form_defs::username, form_defs::password)) {
                main_window = false;
                login_window = false;
            }

            ImGui::End();
        }

        if (register_window) {
            ImGui::SetNextWindowSize({ 300, 200 });

            ImGui::Begin(c_xor("Register Window"), &register_window);
            ImGui::InputTextWithHint(c_xor("##user_box"), c_xor("user"), form_defs::username, IM_ARRAYSIZE(form_defs::username));
            ImGui::InputTextWithHint(c_xor("##email_box"), c_xor("email"), form_defs::email, IM_ARRAYSIZE(form_defs::email));
            ImGui::InputTextWithHint(c_xor("##pass_box"), c_xor("pass"), form_defs::password, IM_ARRAYSIZE(form_defs::password), ImGuiInputTextFlags_Password);
            ImGui::InputTextWithHint(c_xor("##token_box"), c_xor("token"), form_defs::token, IM_ARRAYSIZE(form_defs::token));

            if (ImGui::Button(c_xor("Register")) && c_api::c_register(form_defs::username, form_defs::email, form_defs::password, form_defs::token))
                register_window = false;


            ImGui::End();
        }

        if (activate_window) {
            ImGui::SetNextWindowSize({ 300, 200 });

            ImGui::Begin(c_xor("Activate Window"), &activate_window);
            ImGui::InputTextWithHint(c_xor("##user_box"), c_xor("user"), form_defs::username, IM_ARRAYSIZE(form_defs::username));
            ImGui::InputTextWithHint(c_xor("##pass_box"), c_xor("pass"), form_defs::password, IM_ARRAYSIZE(form_defs::password), ImGuiInputTextFlags_Password);
            ImGui::InputTextWithHint(c_xor("##token_box"), c_xor("token"), form_defs::token, IM_ARRAYSIZE(form_defs::token));

            if (ImGui::Button(c_xor("Activate")) && c_api::c_activate(form_defs::username, form_defs::password, form_defs::token))
                activate_window = false;

            ImGui::End();
        }

        // Rendering
        ImGui::EndFrame();
        g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
        g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
        D3DCOLOR clear_col_dx = D3DCOLOR_RGBA((int)(clear_color.x * 255.0f), (int)(clear_color.y * 255.0f), (int)(clear_color.z * 255.0f), (int)(clear_color.w * 255.0f));
        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, clear_col_dx, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }
        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
            ResetDevice();
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
        return false;

    // Create the D3DDevice
    ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
    g_d3dpp.Windowed = TRUE;
    g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    g_d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
    g_d3dpp.EnableAutoDepthStencil = TRUE;
    g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    //g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;           // Present with vsync
    g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present without vsync, maximum unthrottled framerate
    if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp, &g_pd3dDevice) < 0)
        return false;

    return true;
}

void CleanupDeviceD3D()
{
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    if (g_pD3D) { g_pD3D->Release(); g_pD3D = NULL; }
}

void ResetDevice()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
    HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
    if (hr == D3DERR_INVALIDCALL)
        IM_ASSERT(0);
    ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
