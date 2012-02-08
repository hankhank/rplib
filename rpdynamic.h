#ifndef RPDYNAMIC_H_
#define RPDYNAMIC_H_

#include "rp.h"
#include "rpdebug.h"
#include "rpqueue.h"
#include "rpmap.h"

#include "string.h"

/**
 * Dynamic library factory. C Function that will be loaded and used to 
 * create user classes
 * Use case:
 * 1. Plugin
 * Say we have a class that provides us with a data source and there is a
 * well defined abstract parent class defining the interface, DataSource.
 * DataSource defines the functions Open, ReadDataPacket, Close, Settings, 
 * LoadSettings.
 * We then define a subclass(ie plugin) called 'class FromDisk: public Datasource'
 * Now we can load the subclass like so DataSource* plugin = new RpDynamic("path");
 *
 */
extern "C" void* RpDynFactory();

#define MAXLOADMODULES 20
#define RPDYNLIBVERSION 1

namespace rp 
{
    /**
     * Loadable module interface. 
     * Not an abstract class so we can use rpqueue templates.
     */
    class RpLoadable
    {
    public:
        RpLoadable(){};
        virtual ~RpLoadable(){};
        
        /**
         * Init loadable module
         */
        virtual void _Init() = 0;

        /**
         * Uninit loadable module
         */
        virtual void _UnInit() = 0;
    
        /**
         * Function that returns instance of the loadable class
         */
        virtual void* _Maker() = 0;
        
        /**
         * RpDynamic library version
         */
        int _VersionNum() { return RPDYNLIBVERSION;};

        /**
         * String containing user lib version information
         */
        virtual std::string _LibVersionInfo() = 0;

    };

    /**
     * Load library class
     */
    class RpDynamic
    {
    public:

        enum RpDynamicError { NOERR = 0, FILE_ERR = -1, SYMBOL_ERR = -2 };

        RpDynamic();
        ~RpDynamic();

        /**
         * Register dynamic library. Collects reginfo, init, uninit & maker function 
         * pointers from new library
         */
        RpDynamicError Register(const std::string libpath);

        /**
         */
        RpDynamicError Release(const std::string& index);
        RpDynamicError Release(const RpLoadable& module);
        RpDynamicError Release(const std::string libpath);
        
        /**
         *
         */
        RpDynamicError ReleaseAll();

        /**
         * Overload [] operator to create a "map" of registered plugins. So when
         * you wish to use a plugin you can call it like shape = plugin["type"]
         */
        void* operator[](const std::string& index);

        /**
         * Return list of registered plugins
         */
        int ListLoaded(std::string* pluginsbuf, int num); 

    private:
        RpMap<std::string, RpLoadable*, MAXLOADMODULES> registeredmodules_;
    };
};

#endif

