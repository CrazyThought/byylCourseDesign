#include "mainwindow.h"

#include <QApplication>
#include <windows.h>

// Console application main function
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}

// Windows application entry point (required for some configurations)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    int argc = 0;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    
    if (argv == nullptr) {
        return 1;
    }
    
    // Convert wide strings to char strings
    char **argc_argv = new char*[argc];
    for (int i = 0; i < argc; ++i) {
        int size = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
        argc_argv[i] = new char[size];
        WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, argc_argv[i], size, nullptr, nullptr);
    }
    
    int result = main(argc, argc_argv);
    
    // Cleanup
    for (int i = 0; i < argc; ++i) {
        delete[] argc_argv[i];
    }
    delete[] argc_argv;
    LocalFree(argv);
    
    return result;
}
