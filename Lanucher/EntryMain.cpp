#include "LanucherPCH.h"
#include "IApplication.h"
#include "ShareModule.h"
#include "LanucherConfig.h"

typedef IApplication*(*FNCreateApp)();
typedef void(*FNDestroyApp)(IApplication*);

int main(int argc, char *argv [])
{
    std::string kConfigFile = "init.cfg";
    if (argc > 1) {
        kConfigFile = argv [1];
    }
    CLanucherConfig kConfig(kConfigFile, 0.1);
    if (kConfig.LoadConfig() == false)
        return 0;

    std::string kModuleName = kConfig.GetValueByString("MainModule");
    if (kModuleName.empty())
        kModuleName = "Main";
    CShareModule kModule(kModuleName);
    std::string kCreateApp = kConfig.GetValueByString("CreateApp");
    if (kCreateApp.empty())
        kCreateApp = "LanucherCreateApp";
    FNCreateApp fpCreateApp = kModule.GetAddressByType<FNCreateApp>(kCreateApp);
    if (fpCreateApp == nullptr)
        return -1;
    std::string kDestroyApp = kConfig.GetValueByString("DestroyApp");
    if (kDestroyApp.empty())
        kDestroyApp = "LanucherDestroyApp";
    FNDestroyApp fpDestroyApp = kModule.GetAddressByType<FNDestroyApp>(kDestroyApp);
    if (fpDestroyApp == nullptr)
        return -1;
    IApplication* pkApp = fpCreateApp();
    if (pkApp == nullptr)
        return -1;
    pkApp->Initialize(kConfig);
    pkApp->Run();
    int nReturn = pkApp->Return();
    fpDestroyApp(pkApp);
    return nReturn;
}