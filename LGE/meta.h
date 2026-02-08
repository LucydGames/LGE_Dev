#pragma once

// Renderer Defines
#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
#define THREE_D_VIEW 0
#define TWO_D_VIEW 1
#define RENDER_VIEW 2
#define UI_VIEW 255
// Application Defines
#define APP_NAME "LGE Application"
#define APP_VERSION_MAJOR 0
#define APP_VERSION_MINOR 1
#define APP_VERSION_PATCH 0
#define APP_VERSION_STRING "0.1.0"

#ifdef DEBUG

#endif // DEBUG

#ifdef LGE_PLATFORM_WINDOWS
#ifdef LGE_BUILD_DLL
#define LGE_API __declspec(dllexport)
#else
	#define LGE_API __declspec(dllimport)
#endif
#else
	//#error Lge Currently Only Supports Windows!
#endif // LGE_PLATFORM_WINDOWS

#ifdef LGE_EDITOR_BUILD
//#define SCREEN_WIDTH 1280
#define LGE_USE_IMGUI 0
#else
#define LGE_USE_IMGUI 1
#endif

#define BIT(x) (1 << x)