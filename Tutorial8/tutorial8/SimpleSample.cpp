//--------------------------------------------------------------------------------------
// File: SimpleSample.cpp
//
// This is a simple Win32 desktop application showing use of DirectXTK
//
// http://go.microsoft.com/fwlink/?LinkId=248929
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include <d3d11.h>

#include <directxmath.h>

// DirectXTK includes
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"

#include "resource.h"
#include <sstream>

using namespace DirectX;

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE                           g_hInst = nullptr;
HWND                                g_hWnd = nullptr;
D3D_DRIVER_TYPE                     g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL                   g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*                       g_pd3dDevice = nullptr;
ID3D11DeviceContext*                g_pImmediateContext = nullptr;
IDXGISwapChain*                     g_pSwapChain = nullptr;
ID3D11RenderTargetView*             g_pRenderTargetView = nullptr;
ID3D11Texture2D*                    g_pDepthStencil = nullptr;
ID3D11DepthStencilView*             g_pDepthStencilView = nullptr;

ID3D11InputLayout*                  g_pBatchInputLayout = nullptr;

std::unique_ptr<BasicEffect>                            g_BatchEffect;
std::unique_ptr<GeometricPrimitive>                     g_Shape;
std::unique_ptr<PrimitiveBatch<VertexPositionColor>>    g_Batch;
std::unique_ptr<SpriteBatch>                            g_Sprites;
std::unique_ptr<SpriteFont>                             g_Font;

XMMATRIX                            g_World;
XMMATRIX                            g_View;
XMMATRIX                            g_Projection;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow );
HRESULT InitDevice();
void CleanupDevice();
LRESULT CALLBACK    WndProc( HWND, UINT, WPARAM, LPARAM );
void Render();


//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    UNREFERENCED_PARAMETER( hPrevInstance );
    UNREFERENCED_PARAMETER( lpCmdLine );

    if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
        return 0;

    if( FAILED( InitDevice() ) )
    {
        CleanupDevice();
        return 0;
    }

    // Main message loop
    MSG msg = {0};
    while( WM_QUIT != msg.message )
    {
        if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            Render();
        }
    }

    CleanupDevice();

    return ( int )msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof( WNDCLASSEX );
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon( hInstance, ( LPCTSTR )IDI_SAMPLE1 );
    wcex.hCursor = LoadCursor( nullptr, IDC_ARROW );
    wcex.hbrBackground = ( HBRUSH )( COLOR_WINDOW + 1 );
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"SampleWindowClass";
    wcex.hIconSm = LoadIcon( wcex.hInstance, ( LPCTSTR )IDI_SAMPLE1 );
    if( !RegisterClassEx( &wcex ) )
        return E_FAIL;

    // Create window
    g_hInst = hInstance;
    RECT rc = { 0, 0, 1200, 760};
    AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( L"SampleWindowClass", L"Tutorial 8", WS_OVERLAPPEDWINDOW,
                           CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                           nullptr );
    if( !g_hWnd )
        return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT InitDevice()
{
    HRESULT hr = S_OK;

    RECT rc;
    GetClientRect( g_hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd, sizeof( sd ) );
    sd.BufferCount = 1;
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = g_hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain( nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                            D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &g_featureLevel, &g_pImmediateContext );
        if( SUCCEEDED( hr ) )
            break;
    }
    if( FAILED( hr ) )
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, &g_pRenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, nullptr, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRenderTargetView, g_pDepthStencilView );

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    // Create DirectXTK objects
    g_Sprites.reset( new SpriteBatch( g_pImmediateContext ) );
    g_Batch.reset( new PrimitiveBatch<VertexPositionColor>( g_pImmediateContext ) );

    g_BatchEffect.reset( new BasicEffect( g_pd3dDevice ) );
    g_BatchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        g_BatchEffect->GetVertexShaderBytecode( &shaderByteCode, &byteCodeLength );

        hr = g_pd3dDevice->CreateInputLayout( VertexPositionColor::InputElements,
                                              VertexPositionColor::InputElementCount,
                                              shaderByteCode, byteCodeLength,
                                              &g_pBatchInputLayout );
        if( FAILED( hr ) )
            return hr;
    }

    g_Font.reset( new SpriteFont( g_pd3dDevice, L"italic.spritefont" ) );

    // Initialize the world matrices
    g_World = XMMatrixIdentity();

    // Initialize the view matrix
    XMVECTOR Eye = XMVectorSet( 0.0f, 0.0f, -6.0f, 0.0f );
    XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    g_View = XMMatrixLookAtLH( Eye, At, Up );

    g_BatchEffect->SetView( g_View );

    // Initialize the projection matrix
    g_Projection = XMMatrixPerspectiveFovLH( XM_PIDIV4, width / (FLOAT)height, 0.01f, 100.0f );

    g_BatchEffect->SetProjection( g_Projection );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------
void CleanupDevice()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if ( g_pBatchInputLayout ) g_pBatchInputLayout->Release();

    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    if( g_pDepthStencil ) g_pDepthStencil->Release();
    if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    if( g_pSwapChain ) g_pSwapChain->Release();
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch( message )
    {
        case WM_PAINT:
            hdc = BeginPaint( hWnd, &ps );
            EndPaint( hWnd, &ps );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hWnd, message, wParam, lParam );
    }

    return 0;
}

//--------------------------------------------------------------------------------------
// Render a grid using PrimitiveBatch
//--------------------------------------------------------------------------------------
void DrawGrid()
{
    g_BatchEffect->Apply( g_pImmediateContext );

    g_pImmediateContext->IASetInputLayout( g_pBatchInputLayout );

    g_Batch->Begin();

	int numRows = 10;
	int numColumns = 10;
	int minX = -(numColumns/2);
	int maxX = numColumns/2;
	int minY = -(numRows/2);
	int maxY = numRows/2;

	// TODO: Add code to draw a grid here
	for( int i = minX; i <= maxX; ++i )
    {
		XMFLOAT3 pos1 = XMFLOAT3(i, minY, 0);
		XMFLOAT3 pos2 = XMFLOAT3(i, maxY, 0);
		VertexPositionColor v1(XMLoadFloat3(&pos1), Colors::Gray);
        VertexPositionColor v2(XMLoadFloat3(&pos2), Colors::Gray);
        g_Batch->DrawLine( v1, v2 );
    }
	for( int i = minY; i <= maxY; ++i )
    {
		XMFLOAT3 pos1 = XMFLOAT3(minX, i, 0);
		XMFLOAT3 pos2 = XMFLOAT3(maxX, i, 0);
		VertexPositionColor v1(XMLoadFloat3(&pos1), Colors::Gray);
        VertexPositionColor v2(XMLoadFloat3(&pos2), Colors::Gray);
        g_Batch->DrawLine( v1, v2 );
    }

	XMFLOAT3 yAxis1 = XMFLOAT3(0, maxY, 0);
	XMFLOAT3 yAxis2 = XMFLOAT3(0, minY, 0);
	VertexPositionColor yA1(XMLoadFloat3(&yAxis1), Colors::Red);
	VertexPositionColor yA2(XMLoadFloat3(&yAxis2), Colors::Red);
	g_Batch->DrawLine( yA1, yA2);

	XMFLOAT3 xAxis1 = XMFLOAT3(maxX, 0, 0);
	XMFLOAT3 xAxis2 = XMFLOAT3(minX, 0, 0);
	VertexPositionColor xA1(XMLoadFloat3(&xAxis1), Colors::Red);
	VertexPositionColor xA2(XMLoadFloat3(&xAxis2), Colors::Red);
	g_Batch->DrawLine( xA1, xA2);

    g_Batch->End();
}

//--------------------------------------------------------------------------------------
// Render a frame
//--------------------------------------------------------------------------------------
void Render()
{
    // Update our time
    static float t = 0.0f;
    if( g_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )XM_PI * 0.0125f;
    }
    else
    {
        static uint64_t dwTimeStart = 0;
        uint64_t dwTimeCur = GetTickCount64();
        if( dwTimeStart == 0 )
            dwTimeStart = dwTimeCur;
        t = ( dwTimeCur - dwTimeStart ) / 1000.0f;
    }

	g_World = XMMatrixIdentity();

    //
    // Clear the back buffer
    //
    g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, Colors::MidnightBlue );

    //
    // Clear the depth buffer to 1.0 (max depth)
    //
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	// Draw the grid
	DrawGrid();
	
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	ScreenToClient(g_hWnd, &cursorPos);

	long xLong = cursorPos.x - 600;
	long yLong = -(cursorPos.y - 384);

	std::wostringstream wossX;
	wossX << xLong;
	const std::wstring& xString = wossX.str();
	std::wostringstream wossY;
	wossY << yLong;
	const std::wstring& yString = wossY.str();
	
    // Draw sprite
    g_Sprites->Begin( SpriteSortMode_Deferred );
    
	g_Font->DrawString( g_Sprites.get(), L"0", XMFLOAT2( 600, 384 ), Colors::Yellow );
	g_Font->DrawString( g_Sprites.get(), L"x", XMFLOAT2( 1100, 384 ), Colors::Yellow );
	g_Font->DrawString( g_Sprites.get(), L"y", XMFLOAT2( 600, 0 ), Colors::Yellow );
    
	g_Font->DrawString( g_Sprites.get(), L"x: ", XMFLOAT2( 0, 0 ), Colors::Yellow );
	g_Font->DrawString( g_Sprites.get(), xString.c_str(), XMFLOAT2( 40, 0 ), Colors::Yellow );
	g_Font->DrawString( g_Sprites.get(), L"y: ", XMFLOAT2( 0, 40 ), Colors::Yellow );
	g_Font->DrawString( g_Sprites.get(), yString.c_str(), XMFLOAT2( 40, 40 ), Colors::Yellow );
    
    g_Sprites->End();

    //
    // Present our back buffer to our front buffer
    //
    g_pSwapChain->Present( 0, 0 );
}
