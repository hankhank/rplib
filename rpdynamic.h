#ifndef RPDYNAMIC_H_
#define RPDYNAMIC_H_

#include "rp.h"
#include "rpdebug.h"
#include "rpqueue.h"

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
        ~RpLoadable(){};
        
        /**
         * Init loadable module
         */
        virtual void _Init();

        /**
         * Uninit loadable module
         */
        virtual void _UnInit();
    
        /**
         * Function that returns instance of the loadable class
         */
        virtual void* _Maker();
        
        /**
         * String containing registration information
         */
        virtual std::string Reginfo();
    };

    /**
     * Load library class
     */
    class RpDynamic
    {
        enum RpDynamicError { NOERR };
    public:
        RpDynamic();
        ~RpDynamic();

        /**
         * Register dynamic library. Collects reginfo, init, uninit & maker function 
         * pointers from new library
         */
        RpDynamicError Register(std::string libpath);

        /**
         */
        RpDynamicError Release(RpLoadable& module);
        RpDynamicError Release(std::string libpath);
        
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
        RpQueue<std::string, MAXLOADMODULES> ListLoaded(); 

    private:
        RpQueue<RpLoadable, MAXLOADMODULES> registeredmodules_;
    };
};

#endif

