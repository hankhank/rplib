#ifndef RPMESSAGE_H_
#define RPMESSAGE_H_

namespace rp 
{
    template <typename ENUM_T>
    class RpMessage
    {
    public:
        RpMessage(){};
        RpMessage(ENUM_T type):
            type_(type){};
        RpMessage(const RpMessage& copy):
            type_(copy.type_){};
        ~RpMessage(){};

        ENUM_T GetType(){return type_;};

        bool operator== (const RpMessage<ENUM_T>& rhs)
        {
           return (type_ == rhs.type_);
        };

        bool operator!= (const RpMessage<ENUM_T>& rhs)
        {
           return (type_ != rhs.type_);
        };

        RpMessage<ENUM_T>& operator= (const RpMessage<ENUM_T>& rhs)
        {
           type_ = rhs.type_;
           return *this;
        };

    protected:
        ENUM_T type_;        
    };
    
    template <typename ENUM_T, typename PAYLOAD_T>
    class RpMessageContainer: public RpMessage<ENUM_T>
    {
    public:
        RpMessageContainer(ENUM_T type): 
            RpMessage<ENUM_T>(type){};
        RpMessageContainer(ENUM_T type, PAYLOAD_T payload): 
            RpMessage<ENUM_T>(type), payload_(payload){};
        ~RpMessageContainer(){};

        PAYLOAD_T GetPayload(){return payload_;};

    private:
        PAYLOAD_T payload_;
    };
};

#endif
