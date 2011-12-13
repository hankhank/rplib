#ifndef RPBUFFER_H_
#define RPBUFFER_H_

#include "rpsemaphore.h"
#include "rp.h"
#include "rpdebug.h"

namespace rp 
{
    template <typename T>
    class RpBuffer
    {
    public:
        RpBuffer(T* array, int arraysize);
        ~RpBuffer();
        T* Allocate(); /* Allocate single entry for use */
        int Allocate(T* allocarray[], int num); /* Fill array of pointers 
                                                with pointers */
        
        void Release(T* item); /* Return single item to buffer */
        void Release(T* items[], int num); /* Return multiple to buffer */

        int Available(void); /* Release space available */

    private:
        T* array_;
        int arraysize_;
        bool* used_;
        int available_;
    };

    template <typename T>
    RpBuffer<T>::RpBuffer(T* array, int arraysize): 
        array_(array), arraysize_(arraysize), available_(arraysize)
    {
        used_ = new bool[arraysize_]();
    }

    template <typename T>
    RpBuffer<T>::~RpBuffer()
    {
       delete[] used_;
    }

    /* Allocate single entry for use */
    template <typename T>
    T* RpBuffer<T>::Allocate(void)
    {
        int i = 0;
        for(; i < arraysize_; i++)
        {
            if( !used_[i] )
            {
                available_ -= 1;
                used_[i] = true;
                return &array_[i];
            }
        }
        return NULL;
    }

    /* Fill array of pointers with pointers */
    template <typename T>
    int RpBuffer<T>::Allocate(T* allocarray[], int num)
    {
        int i = 0;
        int ret = 0;
        for(; (i < arraysize_) && (ret < num); i++)
        {
            if( !used_[i] )
            {
                available_ -= 1;
                used_[i] = true;
                allocarray[ret++] = &array_[i];
            }
        }
        return ret;
    }

    /* Return single item to buffer */
    template <typename T>
    void RpBuffer<T>::Release(T* item)
    {
        int i = item - array_ ;
        if( i >= 0 )
        {
            available_ += 1;
            used_[i] = false;
        }
        else
        {
            LOG(WARNING) << "Trying to release item outside buffer " << i;
        }
    }

    /* Return multiple to buffer */
    template <typename T>
    void RpBuffer<T>::Release(T* items[], int num)
    {
        int i = 0;
        for(; i < num; i++)
        {
            Release(items[i]);   
        }
    }

    template <typename T>
    int RpBuffer<T>::Available()
    {
        return available_;
    }
};

#endif
