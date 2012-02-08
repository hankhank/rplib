#include "rpdynamictestlib.h"
#include "rpdynamic.h"
#include "rpdebug.h"
#include "string.h"

using namespace rp;

extern "C"
{
    void* RpLoadableMaker();
}

class RpTestLibClass : public RpTestLibInterface
{
    void Test(){LOG(INFO) << "RpTestLibClass Runs!"; std::cout<<"asd";}
};   

class RpTestLib : public RpLoadable
{
public:
    RpTestLib(){};
    ~RpTestLib(){};
    void _Init(){LOG(INFO) << "_Init";};
    void _UnInit(){LOG(INFO) << "_UnInit";};
    void* _Maker(){LOG(INFO) << "_Maker"; return new RpTestLibClass;};
    std::string _LibVersionInfo(){return std::string(VERSIONSTRING);};
};

void* RpLoadableMaker()
{
    return new RpTestLib;
}
