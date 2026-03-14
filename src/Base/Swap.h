#ifndef BASE_SWAP_H
#define BASE_SWAP_H

#define LOW_ENDIAN (unsigned short)0x4949
#define HIGH_ENDIAN (unsigned short)0x4D4D


namespace Base
{

/**
 * \brief Definition of functions that allow swapping of data types
 * dependent on the architecture.
 */

/** Returns machine type (low endian, high endian) */
unsigned short SwapOrder();

void SwapVar(char&);
void SwapVar(unsigned char&);
void SwapVar(short&);
void SwapVar(unsigned short&);
void SwapVar(long&);
void SwapVar(unsigned long&);
void SwapVar(float&);
void SwapVar(double&);

template<class T>
void SwapEndian(T& v)
{
    T tmp = v;
    int i = 0;

    for (i = 0; i < (int)sizeof(T); i++) {
        *(((char*)&tmp) + i) = *(((char*)&v) + sizeof(T) - i - 1);
    }
    v = tmp;
}

}  // namespace Base


#endif  // BASE_SWAP_H
