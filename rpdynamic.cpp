
#include "rpdebug.h"
#include "rpdynamic.h"
#include "string.h"

#ifdef OS_LINUX
#include "dlfcn.h"
#endif

using namespace rp;

RpDynamic::RpDynamic()
{
}

RpDynamic::~RpDynamic()
{
}

RpDynamic::RpDynamicError RpDynamic::Register(std::string libpath)
{
    void* loadablefile = NULL;
    void* loadablemaker = NULL;
    RpLoadable loadedlib;

    /* Load the maker function and make an instance of the lib */
#ifdef OS_LINUX
    loadablefile = dlopen(libpath.c_str(), RTLD_NOW);
    loadablemaker = dlsym(loadablefile, "RpLoadableMaker");
    loadedlib = static_cast<RpLoadable *()>loadablemaker();
#endif

    /* Init module */
    loadedlib._Init();
    
    /* Add to registered list */
    registeredmodules_.PushBack(loadedlib);
}

RpDynamic::RpDynamicError RpDynamic::ReleaseAll()
{
}

void* RpDynamic::operator[](const std::string& index)
{
}

RpQueue<std::string, MAXLOADMODULES> ListLoaded()
{
}
