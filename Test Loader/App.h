#pragma once
#include <string>

#include "gui.h"

class App
{
public:
    template <class T>
    static T* Create() { return new T(); };

    App(std::string _appName, int _width = Gui::WIDTH, int _height = Gui::HEIGHT);
    virtual ~App();
    
    virtual void Update();

    virtual void BeginRender();
    virtual void Render() = 0;
    virtual void EndRender();
    
private:
    std::string appName;
    int windowWidth;
    int windowHeight;
};
