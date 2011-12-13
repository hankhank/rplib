#ifndef RPMAP_H_
#define RPMAP_H_

#include "rp.h"
#include "rpsemaphore.h"
#include "rpdebug.h"
#include "rpbuffer.h"

namespace rp {

    /**
     * RpMap. Thread safe map of any key type to any value that uses sorted 
     * lists to store data. Keys must have < operator defined.
     */

    /* Template Definition */
    template <typename KEY,typename VALUE, int MAP_SIZE>
    class RpMap
    {

    public:
        RpMap();
        ~RpMap();
        void Insert(KEY newkey, VALUE newvalue);
        void Replace(KEY newkey, VALUE newvalue);

        VALUE& operator[](const KEY& key);
        RpMap<KEY,VALUE, MAP_SIZE>& operator= (const RpMap<KEY,VALUE,MAP_SIZE> &x);
             
        class pair
        {
        public:
            KEY key;
            VALUE* value;
        };

    private:
        int FindKey(KEY key);
        void InsertAtIndex(KEY newkey, VALUE newvalue, int index);
        RpSemaphore *empty_;
        RpSemaphore *lock_;
        RpSemaphore *usd_;
        unsigned int head_;
        unsigned int tail_;
        pair sortedpairs_[MAP_SIZE];
        VALUE mapvalues_[MAP_SIZE];
        RpBuffer<VALUE> *mapvaluesbuf_;
    };

    /* Implementation */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    RpMap<KEY, VALUE, MAP_SIZE>::RpMap()
    {
        empty_ =  new RpSemaphore(MAP_SIZE);
        lock_ =  new RpSemaphore(1);
        usd_ =  new RpSemaphore(0);
        head_ = 0;
        tail_ = 1;
        mapvaluesbuf_ = new RpBuffer<VALUE>(mapvalues_, MAP_SIZE);
    }  

    template <typename KEY, typename VALUE, int MAP_SIZE>
    RpMap<KEY, VALUE, MAP_SIZE>::~RpMap()
    {
        delete empty_;
        delete lock_;
        delete usd_;
        delete mapvaluesbuf_;
    }
    
    /**
     * Returns the index for the key requested. If the key doesn't exist the index
     * where the key can be inserted to retain a sorted list is returned.
     * Not thread safe call.
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    int RpMap<KEY, VALUE, MAP_SIZE>::FindKey(KEY key)
    {
        /* Handle empty case */
        int used = usd_->GetValue();
        if(used == 0)
        {
            LOG(DEBUG) << "Map emtpy";
            return 0;
        }
        else
        {
            /* Binary search for location */
            /* Pick roughly the middle */
            int min = 0;
            int mid = 0;
            int max = used;
            do
            {
                mid = (min + max)/2;
                if(key < sortedpairs_[mid].key)
                {
                    max = mid - 1; /* search the bottom half */
                }
                else /* mid >= sortedpairs_.key[index] */
                {
                    min = mid + 1; /* search the top half */
                }
            } while((sortedpairs_[mid].key != key) && (min <= max));
            return mid;
        }
    }

    /**
     * Inserts new value at specified index
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    void RpMap<KEY, VALUE, MAP_SIZE>::InsertAtIndex(KEY newkey, 
                                                    VALUE value,
                                                    int index)
    {
        LOG(DEBUG) << "Inserting " << newkey << " " << value << "at " << index;
        memmove(&sortedpairs_[index+1], &sortedpairs_[index+1], sizeof(pair));
        sortedpairs_[index].key = newkey;
        sortedpairs_[index].value = mapvaluesbuf_->Allocate();
        *(sortedpairs_[index].value) = value;
        usd_->Post();
    }

    /**
     * Inserts new key, value pair into the map. If key already exists nothing is 
     * done.
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    void RpMap<KEY, VALUE, MAP_SIZE>::Insert(KEY newkey, VALUE value)
    {
        if(empty_->TryWait())
        {
            ContextLock cl(lock_);

            /* Find insertion point */
            int used = usd_->GetValue();
            int index = FindKey(newkey);

            /* Check if insertion point has same key */
            if(sortedpairs_[index].key == newkey)
            {
                LOG(WARNING) << "Key already exists. Nothing added.";
                return;
            }

            /* Insert new pair and store value*/
            InsertAtIndex(newkey, value, index);
        }
        else
        {
            /* Map was full. Perhaps we should return a code or something here */
            LOG(ERROR) << "Map is full. Nothing added.";
            return;
        }
        return;
    }

    template <typename KEY, typename VALUE, int MAP_SIZE>
    VALUE& RpMap<KEY, VALUE, MAP_SIZE>::operator[](const KEY& key)
    {
        ContextLock cl(lock_);
        int index = FindKey(key);
        
        /* Add if value does not exist */
        LOG(DEBUG) << "Looking for key " << key;
        if(sortedpairs_[index].key != key)
        {
            LOG(DEBUG) << "Not found inserting at  " << index;
            VALUE value;
            if(empty_->TryWait())
            {
                InsertAtIndex(key, value, index);
            }
            else
            {
                /* Map was full. Perhaps we should return a code or something here */
                LOG(ERROR) << "Map is full. Nothing added.";
            }
        }

        return *(sortedpairs_[index].value);
    }

    template <typename KEY, typename VALUE, int MAP_SIZE>
    RpMap<KEY, VALUE, MAP_SIZE>& 
        RpMap<KEY,VALUE, MAP_SIZE>::operator= (const RpMap<KEY,VALUE,MAP_SIZE> &x)
    {
        LOG(ERROR) << "Not implemented";
        ContextLock cl(lock_);
        RpMap<KEY, VALUE, MAP_SIZE> retmap;
        return retmap;
    }

}
#endif
