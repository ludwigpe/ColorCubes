#include <Windows.h>
#include <d3dx9.h>
#include <sstream>
#include <string>
#include <iterator>
#include <vector>
#include <time.h>
#include <stdlib.h>     /* srand, rand */
using namespace std;

#include "InputController.h"
#include "GameBoard.h"
#include "Mesh.h"
#include "TextureManager.h"
#include "MeshManager.h"

const int BOARD_SIZE = 10;
const float FOV = D3DX_PI/2;
const bool FULLSCREEN = false;

bool StartDirect3D(HWND window, bool fullscreen);
void InitialiseCamera();
void ShutdownDirect3D();
void Update();
void Render();
void SetupCamera();

LPDIRECT3D9 g_D3D = NULL;
LPDIRECT3DDEVICE9 g_D3DDevice = NULL;
InputController* g_inputController = NULL;
LPD3DXFONT g_Font = NULL;

int g_WinWidth = 1280;
int g_WinHeight = 720;
float angle;

GameBoard* g_gameBoard;

D3DXVECTOR3 cameraPos(BOARD_SIZE/2, BOARD_SIZE/(2*tan(FOV/2)) + 3, 0);

D3DXVECTOR3 Eye(0.0f, 1.5f, -10.0f);
D3DXVECTOR3 LookAt(BOARD_SIZE/2	, 0.0f, BOARD_SIZE/2);
D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);


LRESULT WINAPI GameWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_KEYDOWN:
		g_inputController->SetKeyDown(wParam);
		return 0;
	case WM_KEYUP:
		g_inputController->SetKeyUp(wParam);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX gameWindowClass;
	memset(&gameWindowClass, 0, sizeof(gameWindowClass));

	gameWindowClass.cbSize = sizeof(gameWindowClass);
	gameWindowClass.style = CS_CLASSDC;
	gameWindowClass.lpfnWndProc = &GameWindowProc;
	gameWindowClass.hInstance = hInstance;
	gameWindowClass.lpszClassName = "GameWindow1";
	gameWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	RegisterClassEx(&gameWindowClass);

	HWND gameWindowHandle = CreateWindow(
		"GameWindow1",
		"Assignment 1",
		WS_OVERLAPPEDWINDOW,
		100, 100,
		g_WinWidth, g_WinHeight,
		GetDesktopWindow(),
		NULL,
		hInstance,
		NULL
		);

	bool running = true;

	if(StartDirect3D(gameWindowHandle, FULLSCREEN))
	{
		g_inputController = new InputController(gameWindowHandle);
		g_gameBoard = new GameBoard(g_D3DDevice, BOARD_SIZE, g_inputController, g_Font);
		g_gameBoard->SetupAssets();
		ShowWindow(gameWindowHandle, SW_SHOW);
		MSG message;
		while(running)
		{
			if(PeekMessage(&message,  NULL, NULL, NULL, PM_REMOVE))
			{
				if(message.message == WM_QUIT)
				{
					running = false;
				}
				TranslateMessage(&message);
				DispatchMessage(&message);
			}
			else
			{
				Update();
				Render();
			}
		}
		ShutdownDirect3D();

	}
	else
	{
		MessageBox(NULL, "Unable to initialise Direct 3D, is Direct X installed?", "Error", MB_OK);
	}

	UnregisterClass("GameWindow1", hInstance);

	return 0;
}

bool StartDirect3D(HWND window, bool fullscreen)
{
	if((g_D3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
	{
		return false;
	}

	D3DDISPLAYMODE displayMode;
	g_D3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode);

	D3DPRESENT_PARAMETERS presentParams;
	memset(&presentParams, 0, sizeof(presentParams));

	presentParams.BackBufferFormat = displayMode.Format;
	presentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
	presentParams.BackBufferCount = 1;
	presentParams.EnableAutoDepthStencil = TRUE;
	presentParams.AutoDepthStencilFormat = D3DFMT_D16;
	if(fullscreen)
	{
		presentParams.BackBufferWidth = displayMode.Width;
		presentParams.BackBufferHeight = displayMode.Height;
		g_WinWidth = displayMode.Width;
		g_WinHeight = displayMode.Height;
		presentParams.Windowed = false;
	}
	else
	{
		presentParams.Windowed = true;
	}

	if(FAILED(g_D3D->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		window,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&presentParams,
		&g_D3DDevice
		)))
	{
		return false;
	}

	g_D3DDevice->SetRenderState(D3DRS_ZENABLE, true);
	//g_D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_D3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	if(FAILED(D3DXCreateFont(
		g_D3DDevice,
		24,
		0,
		0,
		1,
		0,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		"Arial",
		&g_Font
		)))
	{
		return false;
	}
	InitialiseCamera();


	return true;
}

void InitialiseCamera()
{
	D3DXMATRIX Projection;

	D3DXMatrixPerspectiveFovLH(&Projection, FOV, g_WinWidth/g_WinHeight, 0.1f, 500.0f);
	g_D3DDevice->SetTransform(D3DTS_PROJECTION, &Projection);
	
	

	D3DXMATRIX View;

	D3DXMatrixLookAtLH(&View, &Eye, &LookAt, &Up);

	g_D3DDevice->SetTransform(D3DTS_VIEW, &View);

}

void ShutdownDirect3D()
{
	if(g_gameBoard)
	{
		g_gameBoard->Release();
		g_gameBoard = NULL;
		
	}
	if(g_D3DDevice)
	{
		g_D3DDevice->Release();
		g_D3DDevice = NULL;
	}
	if(g_D3D)
	{
		g_D3D->Release();
		g_D3D = NULL;
	}
	if(g_Font)
	{
		g_Font->Release();
		g_Font = NULL;
	}

	delete g_inputController;
}

bool spacePressed = false;

void Update()
{

	g_inputController->BeginUpdate();

	if(g_inputController->GetKeyUp(VK_ESCAPE))
	{
		PostQuitMessage(0);
	}

	SetupCamera();
	g_gameBoard->Update();
	g_inputController->EndUpdate();

	


}

void Render()
{
	g_D3DDevice->Clear(0, NULL, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER), D3DCOLOR_ARGB(0, 128, 128, 128), 1.0f, 0);

	g_D3DDevice->BeginScene();
	// Draw here!


	g_gameBoard->Render();
	g_D3DDevice->EndScene();

	g_D3DDevice->Present(NULL, NULL, NULL, NULL);
}

void SetupCamera()
{
	D3DXMATRIX View;
	D3DXMatrixLookAtLH(&View, &cameraPos, &LookAt, &Up);
	g_D3DDevice->SetTransform(D3DTS_VIEW, &View);
}
