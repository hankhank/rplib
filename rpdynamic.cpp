
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
    ReleaseAll();
}

RpDynamic::RpDynamicError RpDynamic::Register(const std::string libpath)
{
    void* loadablefile = NULL;
    void* loadablemaker = NULL;
    RpLoadable* loadedlib = NULL;
    std::string libversion;
    
    /* Load the maker function and make an instance of the lib */
#ifdef OS_LINUX
    loadablefile = dlopen(libpath.c_str(), RTLD_NOW);
    if(loadablefile == NULL)
    {
        LOG(ERROR) << "Could not open loadable file " << libpath << " "
            << dlerror();
        return FILE_ERR;
    }

    loadablemaker = dlsym(loadablefile, "RpLoadableMaker");
    if(loadablemaker == NULL)
    {
        LOG(ERROR) << "Could not find expected RpLoadableMaker symbol";
        return SYMBOL_ERR;
    }
    loadedlib = reinterpret_cast<RpLoadable* (*)()> (loadablemaker)();
#endif

    /* Init module */
    loadedlib->_Init();
    libversion.assign(loadedlib->_LibVersionInfo());

    /* Add to registered list */
    registeredmodules_[libversion] = loadedlib;
    return NOERR;
}

RpDynamic::RpDynamicError RpDynamic::Release(const std::string& index)
{
}

RpDynamic::RpDynamicError RpDynamic::Release(const RpLoadable& module)
{
}

RpDynamic::RpDynamicError RpDynamic::Release(const std::string libpath)
{
}
     
RpDynamic::RpDynamicError RpDynamic::ReleaseAll()
{
    std::string loadedmodules[MAXLOADMODULES];
    int i = 0;
    int loaded = registeredmodules_.Keys(loadedmodules, MAXLOADMODULES);

    for(; i < loaded; i++)
    {
        RpLoadable* lib = registeredmodules_[loadedmodules[i]];
        lib->_UnInit();
        delete lib;
    }
}

void* RpDynamic::operator[](const std::string& index)
{
    int i = registeredmodules_.Find(index);
    if( i < 0 )
    {
        LOG(ERROR) << "Module " << index << " not registered";
        return NULL;
    }
    RpLoadable* lib = registeredmodules_.Index(i);
    return lib->_Maker();
}

int RpDynamic::ListLoaded(std::string* pluginsbuf, int num)
{
    return registeredmodules_.Keys(pluginsbuf, num);
}
