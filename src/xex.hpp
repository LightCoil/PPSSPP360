#ifndef XEX_HPP
#define XEX_HPP

#include <string>
#include <memory>

namespace Xbox360 {

struct XEX_MODULE_INFO {
    std::string name;
    std::string path;
    bool isLoaded;
};

class Xex {
public:
    Xex();
    ~Xex();

    bool Initialize();
    void Shutdown();
    bool LoadModule(const char* path, XEX_MODULE_INFO* info);
    bool UnloadModule(const char* name);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace Xbox360

#endif // XEX_HPP 