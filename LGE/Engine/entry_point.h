#pragma once

// #ifdef LGE_PLATFORM_WINDOWS

extern lge::Application* lge::CreateApplication();

int main()
{
	auto application = lge::CreateApplication();
	application()->Run();
	delete application;
	return 0;
}