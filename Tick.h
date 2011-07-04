#ifndef TICK_H
#define TICK_H

class Tick
{
public:
    static bool less(unsigned int a, unsigned int b)
    {
        return (a < b) && (b - a < TICK_MAX_HALF);
    }

private:
    static const unsigned int TICK_MAX = 0xffffffff;
    static const unsigned int TICK_MAX_HALF = 0x7fffffff;
};

#endif // TICK_H
