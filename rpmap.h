#ifndef RPMAP_H_
#define RPMAP_H_

#include "rp.h"
#include "rpsemaphore.h"
#include "rpdebug.h"
#include "rpbuffer.h"

namespace rp {

    /**
     * @class RpMap. 
     * @brief Thread safe map of any key type to any value that uses sorted 
     *        array to store data. Keys must have < operator defined.
     *
     * @tparam KEY      Type for the map key
     * @tparam VALUE    Type of values to be stored in map
     * @tparam MAP_SIZE Max number of items that can be stored in map. 
     *                  Allocated at map creation.
     */
    template <typename KEY,typename VALUE, int MAP_SIZE>
    class RpMap
    {

    public:
        RpMap();
        ~RpMap();
       
        int Insert(const KEY& newkey, VALUE& newvalue);
        int Erase(const KEY& newkey);

        int Keys(KEY* keybuf, int size);
        
        int Find(const KEY& keytofind);
        VALUE& Index(int index);

        VALUE& operator[](const KEY& key);
        RpMap<KEY,VALUE, MAP_SIZE>& operator= (const RpMap<KEY,VALUE,MAP_SIZE> &x);
    
    private:
        int FindKey(const KEY& key);
        void PrintArray();
        void InsertAtIndex(const KEY& newkey, VALUE& newvalue, int index);
        int EraseAtIndex(int index);

        RpSemaphore *empty_; /**< Empty count*/
        RpSemaphore *lock_;
        RpSemaphore *usd_;

        struct pair
        {
            KEY key;
            VALUE* value;
        };
        struct pair sortedpairs_[MAP_SIZE];
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
     * @brief   Returns the index for the key requested. If the 
     *          key doesn't exist the index where the key can be 
     *          inserted to retain a sorted list is returned.
     *          Not thread safe call.
     * @param   key Key value to find 
     * 
     * @return  Index where the key was found in the underlying 
     *          array.
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    int RpMap<KEY, VALUE, MAP_SIZE>::FindKey(const KEY& key)
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
     * @brief   Inserts new value at specified index. 
     *
     * @param   newkey  The new key to identify the value with.
     * @param   value   New value to be inserted.
     * @param   index   Index where in the array to insert
     *
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    void RpMap<KEY, VALUE, MAP_SIZE>::InsertAtIndex(const KEY& newkey, 
                                                    VALUE& value,
                                                    int index)
    {
        int used = usd_->GetValue();
        if(index < (used))
        {
            memmove(&(sortedpairs_[index+1]), &(sortedpairs_[index]), 
                (used-index) * sizeof(struct pair));
        }
        sortedpairs_[index].key = newkey;
        sortedpairs_[index].value = mapvaluesbuf_->Allocate();
        *(sortedpairs_[index].value) = value;
        usd_->Post();
        return;
    }

    /**
     * @brief   Inserts new key, value pair into the map. If key already 
     *          exists nothing is done.
     * @param   newkey  Newkey idenify the value.
     * @param   value   Value to be inserted.
     *
     * @return  Index where the item was inserted. < 0 if the value wasn't
     *          inserted because map was full or key already exsisted.
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    int RpMap<KEY, VALUE, MAP_SIZE>::Insert(const KEY& newkey, VALUE& value)
    {
        int index = -1;
        if(empty_->TryWait())
        {
            ContextLock cl(lock_);

            /* Find insertion point */
            int used = usd_->GetValue();
            index = FindKey(newkey);

            /* Check if insertion point has same key */
            if((sortedpairs_[index].key == newkey) && 
                (sortedpairs_[index].value != NULL))
            {
                LOG(WARNING) << "Key already exists. Nothing added.";
                return -1;
            }

            /* Insert new pair and store value*/
            InsertAtIndex(newkey, value, index);
        }
        else
        {
            /* Map was full. Perhaps we should return a code or something here */
            LOG(ERROR) << "Map is full. Nothing added.";
        }
        return index;
    }

    /**
     * @brief   
     *
     * @param   
     * @param  
     *
     * @return  
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    int RpMap<KEY, VALUE, MAP_SIZE>::Erase(const KEY& key)
    {
        int index = Find(key);
        if( index < 0 )
        {
            return index;
        }
        return EraseAtIndex(index);
    }

    /**
     * @brief   
     *
     * @param   
     * @param  
     *
     * @return  
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    int RpMap<KEY, VALUE, MAP_SIZE>::EraseAtIndex(int index)
    {
        int used = usd_->GetValue();
        if( index < used)
        {
            mapvaluesbuf_->Release(sortedpairs_[index].value);
            usd_->TryWait();
            empty_->Post();
            memmove(&(sortedpairs_[index]), &(sortedpairs_[index+1]), 
                (used-index-1) * sizeof(struct pair));
        }
        else
        {
            LOG(ERROR) << "Index does not exist. No changes to map";
            index = -1;
        }
        return index;
    }

    /**
     * @brief   Return a copy of the maps keys into the buf passed 
     *          in by the caller
     *
     * @param   keybuf  Buffer to hold returned keys
     * @param   size    Number of elements buffer can hold. 
     *                  Not in bytes.
     *
     * @return  Number of keys copied into the buffer.
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    int RpMap<KEY, VALUE, MAP_SIZE>::Keys(KEY* keybuf, int size)
    {
        ContextLock cl(lock_);
        int i, used = usd_->GetValue();
        if( used < size )
        {
            size = used;
        }

        /* Have to use loop as we have an array of structs */
        for(i = 0; i < size; i++)
        {
            keybuf[i] = sortedpairs_[i].key;
        }
        return size;
    }

    /**
     * @brief   Returns the index for a particular key in map.
     *
     * @param   keytofind   Key to find in map.
     *
     * @return  Index of key found in map or < 0 if not found.
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    int RpMap<KEY, VALUE, MAP_SIZE>::Find(const KEY& keytofind)
    {
        ContextLock cl(lock_);
        int index = FindKey(keytofind);
        if( keytofind == sortedpairs_[index].key )
        {
            return index;
        }
        return -1;
    }

    /**
     * @brief   Returns the value of map at a particular index.
     *          Not safe if index outside map is used.
     *
     * @param  index    Index of value to return
     *
     * @return  Returns requested value
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    VALUE& RpMap<KEY, VALUE, MAP_SIZE>::Index(int index)
    {
        ContextLock cl(lock_);
        int used = usd_->GetValue();
        if( used <= index )
        {
            LOG(ERROR) << "Index not in map. return value anyway";
        }
        return *(sortedpairs_[index].value);
    }

    /**
     * @brief   Map access operator. If key doesn't exsist new item
     *          using default constructor is added to map.
     *
     * @param   key Key of element to add/create
     *
     * @return  Value of key returned.
     *
     */
    template <typename KEY, typename VALUE, int MAP_SIZE>
    VALUE& RpMap<KEY, VALUE, MAP_SIZE>::operator[](const KEY& key)
    {
        ContextLock cl(lock_);
        int index = FindKey(key);
        int used = usd_->GetValue();

        /* Add if value does not exist */
        if((used <= index ) || (sortedpairs_[index].key != key))
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
