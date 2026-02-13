#include <lge.h>

class EditorApplication : public lge::Application
{
public:
	EditorApplication() 
	{

	}
	~EditorApplication() 
	{

	}
};

lge::Application* lge::CreateApplication()
{
	return new EditorApplication();
}