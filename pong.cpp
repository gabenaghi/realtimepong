#include "mbed.h"
#include "rtos.h"

Serial pc(USBTX, USBRX);

int main()
{
    pc.printf("hello, world!");
}
