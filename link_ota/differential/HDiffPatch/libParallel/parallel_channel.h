//  parallel_channel.h
/*
 The MIT License (MIT)
 Copyright (c) 2018 HouSisong
 
 Permission is hereby granted, free of charge, to any person
 obtaining a copy of this software and associated documentation
 files (the "Software"), to deal in the Software without
 restriction, including without limitation the rights to use,
 copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following
 conditions:
 
 The above copyright notice and this permission notice shall be
 included in all copies of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef parallel_channel_h
#define parallel_channel_h
#include "parallel_import.h"
#if (_IS_USED_MULTITHREAD)

#include <stddef.h> //for size_t ptrdiff_t

struct CHLocker{
    HLocker locker;
    inline CHLocker():locker(0) { locker=locker_new(); }
    inline ~CHLocker() { locker_delete(locker); }
};

#if (_IS_USED_CPP11THREAD)
#   include <thread>
    struct CAutoLocker:public _TLockerBox_name {
        inline CAutoLocker(HLocker _locker)
            :_TLockerBox_name(*(std::mutex*)_locker){ }
        inline ~CAutoLocker(){ }
    };
#else
    struct CAutoLocker:public TLockerBox {
        inline CAutoLocker(HLocker _locker){ locker=_locker; locker_enter(locker); }
        inline ~CAutoLocker(){ locker_leave(locker); }
    };
#endif

    //通道交互数据;
    typedef void* TChanData;

    class _CChannel_import;
    //通道;
    class CChannel{
    public:
        explicit CChannel(ptrdiff_t maxDataCount=-1);
        ~CChannel();
        void close();
        bool is_can_fast_send(bool isWait); //mybe not need wait when send
        bool send(TChanData data,bool isWait); //can't send null
        TChanData accept(bool isWait); //result can null;
    private:
        _CChannel_import* _import;
    };

#endif //_IS_USED_MULTITHREAD
#endif //parallel_channel_h
