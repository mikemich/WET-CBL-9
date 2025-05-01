#include "app.hpp"
#include "serial.hpp"

extern "C"
{

    void setup()
    {
        Serial_printf("Application starting...\n");
        int test_digit = 1;
        while (1)
        {
            // Infinite loop
            Serial_printf("Application working ...%d\n", test_digit++);

            HAL_Delay(1000); // Delay for 1 second
        }
    }
}
