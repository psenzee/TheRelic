#ifndef _ILUASERIALIZABLE_H
#define _ILUASERIALIZABLE_H

class ILuaSerializable
{
public:
    virtual ~ILuaSerializable() {}
    
    virtual bool IsSerializable() const = 0;
    
    virtual void Serialize(/** stream abstraction, or raw memory pointer* */) = 0;
};

class ILuaDeserializeFactory
{
public:
    virtual ~ILuaDeserializeFactory() {}
    
    virtual ILuaSerializable *Deserialize(/** stream abstraction, or raw memory pointer* */) = 0;
};

#endif  // _ILUASERIALIZABLE_H