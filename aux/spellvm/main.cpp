#include <string.h>
#include <vector>
#include <conio.h>
#include <assert.h>

/*

spell (stack-based) vm [kinda forth-like]
(use this to author built-in spells as well)
runic - each rune corresponds to an opcode
- you acquire/unlock new runes throughout the game
- cheat code to acquire/unlock a new rune?
- including runes for constants such as 0, 1, 2, 3, 5, 8, 13, 21, 34, 55 (fib series)
- including runes for variables a, b, c, d, e, f, ..
- some of the last runes to unlock: +, -, /, *

- programs (spells) can have a name, shorter ones can be their own names, longer ones may need a different name
- programs (spells) are saved in a character profile

examples
1 = push constant
C = getclosestenemy within (pop) [lower and higher power versions of getclosestenemy]
1 = push constant
A = air damage (pop) (pop)

21 = push constant
X = getanyenemy within (pop) [lower and higher power versions of getanyenemy]
55 = push constant
E = earth damage (pop) (pop)

1 = push constant
L = enemylistenumerator within (pop) [lower and higher power versions of enemylistenumerator]
: - push ip
*2 = duplicate second to top item
C = getnext within (pop)
1 = push constant
F = fire damage (pop) (pop)
> = goto (pop)

A DEBUG MODE, THAT SHOWS EXECUTION?

----------------------

some opcodes
[constant pushes]
push ip
pop
swap top items
duplicate top item (top)
duplicate second to top item (top2)
earth damage (pop damage) (pop character)
fire damage (pop damage) (pop character)
water damage (pop damage) (pop character)
air damage (pop damage) (pop character)
getanyenemy within (pop) [lower and higher power versions]
getanycharacter within (pop) [lower and higher power versions]
getanyplayer within (pop) [lower and higher power versions]
getclosestenemy within (pop) [lower and higher power versions]
getclosestplayer within (pop) [lower and higher power versions]
getclosestcharacter within (pop) [lower and higher power versions]
enemyenumerator within (pop) [lower and higher power versions]
playerenumerator within (pop) [lower and higher power versions]
characterenumerator within (pop) [lower and higher power versions]
projectile to (pop character) (pop effect-type)
goto (pop ip)
goto until iterator empty (pop ip) (top enumerator)
(pop) ? (pop) : (pop)
fx (pop) - type of visual effect, a number
move

others
------
slide (accelerometer)
drag (with touch)
rumble (iphone?)
flash
sound fx (pop) - type of sound effect, a number
shake screen (pop time) (pop intensity)
reduce (pop character) <attribute: experience, strength, magic, HP, etc..>
steal (pop character) <attribute: experience, strength, magic, HP, etc..>
increase (pop character) <attribute: experience, strength, magic, HP, etc..>
follower (create a skelly follower)
teleport

*/

class Character;

class Value
{
public:

    enum Type { NONE, NUMBER, IP, CHARACTER /*, etc.. */ };

    inline Value()                     : mType(NONE), mCharacter(0) {}
    inline Value(short      ip)        : mType(NONE) { Set(ip); }
    inline Value(float      number)    : mType(NONE) { Set(number); }
    inline Value(Character *character) : mType(NONE) { Set(character); }
    inline Value(const Value &other)                 { memcpy(this, &other, sizeof(Value)); }

    inline Value &operator=(const Value &other) { memcpy(this, &other, sizeof(Value)); return *this; }

    inline Type       GetType()      const      { return mType; }

    inline void       Set(float number)         { mNumber    = number;    mType = NUMBER; }
    inline void       Set(short ip)             { mIp        = ip;        mType = IP; }
    inline void       Set(Character *character) { mCharacter = character; mType = CHARACTER; }

    inline float      GetNumber()    const      { return (mType != NUMBER)    ? 0.f : mNumber; }
    inline short      GetIp()        const      { return (mType != IP)        ? 0 : mIp; }
    inline Character *GetCharacter() const      { return (mType != CHARACTER) ? 0 : mCharacter; }

private:

    Type mType;
    union
    {
        float              mNumber;
        short              mIp;
        mutable Character *mCharacter;
    };
};

class Function
{
public:

    typedef Value (*fn0r_t)();
    typedef void  (*fn0_t)();
    typedef Value (*fn1r_t)(const Value &);
    typedef void  (*fn1_t)(const Value &);
    typedef Value (*fn2r_t)(const Value &, const Value &);
    typedef void  (*fn2_t)(const Value &, const Value &);
    typedef Value (*fn3r_t)(const Value &, const Value &, const Value &);
    typedef void  (*fn3_t)(const Value &, const Value &, const Value &);

    Function() { memset(this, 0, sizeof(*this)); }

    void Set(fn0_t fn)  { mArity = ARITY_0;  mFn0  = fn; }
    void Set(fn0r_t fn) { mArity = ARITY_0R; mFn0r = fn; }
    void Set(fn1_t fn)  { mArity = ARITY_1;  mFn1  = fn; }
    void Set(fn1r_t fn) { mArity = ARITY_1R; mFn1r = fn; }
    void Set(fn2_t fn)  { mArity = ARITY_2;  mFn2  = fn; }
    void Set(fn2r_t fn) { mArity = ARITY_2R; mFn2r = fn; }
    void Set(fn3_t fn)  { mArity = ARITY_2;  mFn3  = fn; }
    void Set(fn3r_t fn) { mArity = ARITY_2R; mFn3r = fn; }

    Value Call(std::vector<Value> &stack)
    {
        Value r, v1, v2, v3;
        switch (mArity)
        {
        case ARITY_3:  v3 = Pop(stack); v2 = Pop(stack); v1 = Pop(stack); mFn3(v1, v2, v3); break;
        case ARITY_3R: v3 = Pop(stack); v2 = Pop(stack); v1 = Pop(stack); r = mFn3r(v1, v2, v3); break;
        case ARITY_2:  v2 = Pop(stack); v1 = Pop(stack); mFn2(v1, v2); break;
        case ARITY_2R: v2 = Pop(stack); v1 = Pop(stack); r = mFn2r(v1, v2); break;
        case ARITY_1:  v1 = Pop(stack); mFn1(v1); break;
        case ARITY_1R: v1 = Pop(stack); r = mFn1r(v1); break;
        case ARITY_0:  mFn0(); break;
        case ARITY_0R: r = mFn0r(); break;
        }
        return r;
    }

private:

    enum Arity { ARITY_0, ARITY_0R, ARITY_1, ARITY_1R, ARITY_2, ARITY_2R, ARITY_3, ARITY_3R };

    Arity mArity;
    union
    {
        fn0_t  mFn0;
        fn0r_t mFn0r;
        fn1_t  mFn1;
        fn1r_t mFn1r;
        fn2_t  mFn2;
        fn2r_t mFn2r;
        fn3_t  mFn3;
        fn3r_t mFn3r;
    };

    inline static Value Pop(std::vector<Value> &stack) { Value v = stack.back(); stack.pop_back(); return v; }
};

class SpellVm
{
public:

    SpellVm() : mInstruction(0), mProgram(0) {}

    void RegisterFunction(int index, Function function) { mFunctions[index] = function; }

    void Execute(const char *program)
    {
        mProgram = program;
        mInstruction = 0;
        while (ExecuteOne())
        {
        }
        assert(mStack.empty());
        mProgram = 0;
        mInstruction = 0;
    }

private:

    enum { END = 0, POP = 'A', DUP, SWAP, JMP, JL, JG, JE, PUSH_IP, PUSH_NEXT_IP, EXEC_START = 'a' };

    bool ExecuteOne();
    void Jump();
    int  Compare();

    inline void  Push(const Value &value) { mStack.push_back(value); }
    inline Value Top()                    { return mStack.back(); }
    inline Value Pop()
    { 
        if (mStack.empty())
        {
            // error!
            // handle properly
            assert(false);
        }        
        Value v = mStack.back();
        mStack.pop_back();
        return v;
    }

    std::vector<Value>    mStack;
    short                 mInstruction;
    Function              mFunctions[128];
    const char           *mProgram;
};

template <typename FunctionT>
inline void RegisterFunction(SpellVm &vm, int index, FunctionT fn)
{
    Function f;
    f.Set(fn);
    vm.RegisterFunction(index, f);
}

void SpellVm::Jump()
{
    Value v = Pop();
    if (v.GetType() != Value::IP)
    {
        // error!
        // deal with it properly
        assert(false);
    }
    mInstruction = v.GetIp();
}

int SpellVm::Compare()
{
    Value u = Pop(), v = Pop();
    if (u.GetType() != Value::NUMBER ||
        v.GetType() != Value::NUMBER)
    {
        // error!
        // deal with it properly
        assert(false);
    }
    float value = u.GetNumber() - v.GetNumber();
    if      (value < 0.f) return -1;
    else if (value > 0.f) return 1;
    return 0;
}

bool SpellVm::ExecuteOne()
{
    int op = mProgram[mInstruction++];
    if (op >= EXEC_START)
    {
        Value r = mFunctions[op - EXEC_START].Call(mStack);
        if (r.GetType() != Value::NONE)
            Push(r);
        return true;
    }
    else
    {
        int c = 0;
        switch (op)
        {
        case END:                                                                           return false;
        case POP:          Pop();                                                           return true;
        case DUP:          Push(Top());                                                     return true;
        case SWAP:         std::swap(mStack[mStack.size() - 1], mStack[mStack.size() - 2]); return true;
        case JMP:          mInstruction = Pop().GetIp();                                    return true;
        case JL:           c = Compare(); if (c <  0) Jump();                               return true;
        case JG:           c = Compare(); if (c >  0) Jump();                               return true;
        case JE:           c = Compare(); if (c == 0) Jump();                               return true;
        case PUSH_IP:      Push(Value(short(mInstruction - 1)));                            return true;
        case PUSH_NEXT_IP: Push(Value(mInstruction));                                       return true;
        }
    }
    return false;
}

void Print(const Value &v)
{
    switch (v.GetType())
    {
    case Value::CHARACTER: printf("Character %p\n", v.GetCharacter()); break;
    case Value::NUMBER:    printf("Number %.2f\n", v.GetNumber()); break;
    case Value::IP:        printf("IP %d\n", v.GetIp()); break;
    case Value::NONE:      printf("<none>\n"); break;
    }
}

// need to do type checking here
Value Add(const Value &u, const Value &v) { return Value(u.GetNumber() + v.GetNumber()); }
Value Mul(const Value &u, const Value &v) { return Value(u.GetNumber() * v.GetNumber()); }
Value Div(const Value &u, const Value &v) { return Value(u.GetNumber() / v.GetNumber()); }
Value Sub(const Value &u, const Value &v) { return Value(u.GetNumber() - v.GetNumber()); }

Value Get42()
{
    return Value(42.0f);
}

void main()
{
    SpellVm vm;
    RegisterFunction(vm, 0, Print);
    RegisterFunction(vm, 1, Get42);
    RegisterFunction(vm, 2, Add);
    RegisterFunction(vm, 3, Mul);
    RegisterFunction(vm, 4, Div);
    RegisterFunction(vm, 5, Sub);
    vm.Execute("HbbcbdaD");
    getch();
}
