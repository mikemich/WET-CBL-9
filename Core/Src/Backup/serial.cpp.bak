#include <serial.hpp>
#include <usart.h>
#include <cstdlib> // for atoi
#include <cstring> // for strlen
#include <cstdarg> // for va_list

extern UART_HandleTypeDef huart2;

#define UART_TIMEOUT 1000 // 1 second

// Helper function to convert an integer to a string (base 10)
void my_itoa(int num, char *str, int base)
{
    int i = 0;
    bool isNegative = false;

    // Handle 0 explicitly
    if (num == 0)
    {
        str[i++] = '0';
        str[i] = '\0';
        return;
    }

    // Handle negative numbers only if base is 10
    if (num < 0 && base == 10)
    {
        isNegative = true;
        num = -num;
    }

    // Process individual digits
    while (num != 0)
    {
        int rem = num % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        num = num / base;
    }

    // Append negative sign for negative numbers
    if (isNegative)
    {
        str[i++] = '-';
    }

    str[i] = '\0'; // Null-terminate string

    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end)
    {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

void my_ftoa(float num, char *str, int precision)
{
    int int_part = (int)num;
    float frac_part = num - int_part;
    char int_str[20];
    my_itoa(int_part, int_str, 10);

    // Copy the integer part to the string
    int i = 0;
    while (int_str[i] != '\0')
    {
        str[i] = int_str[i];
        i++;
    }

    // Add the decimal point
    str[i++] = '.';

    // Handle the fractional part
    for (int j = 0; j < precision; j++)
    {
        frac_part *= 10;
        int frac_digit = (int)frac_part;
        str[i++] = (frac_digit) + '0';
        frac_part -= frac_digit;
    }

    str[i] = '\0';
}

// A simple function to manually process variable arguments and format
void Serial_Printf_Char(char *buffer, const char *format, va_list args)
{
    char *str = buffer;
    const char *p = format;

    while (*p != '\0')
    {
        if (*p == '%') // Format specifier found
        {
            p++;
            switch (*p)
            {
            case 'd': // Handle integer (decimal)
            {
                int num = va_arg(args, int);
                char temp[20];
                my_itoa(num, temp, 10); // Convert int to string
                for (int i = 0; temp[i] != '\0'; i++)
                {
                    *str++ = temp[i];
                }
                break;
            }
            case 's': // Handle string
            {

                const char *str_arg = va_arg(args, const char *);
                while (*str_arg)
                {
                    *str++ = *str_arg++;
                }
                break;
            }

            case 'c': // Handle character
            {
                char c = va_arg(args, int); // char is promoted to int
                *str++ = c;
                break;
            }
            // Add more cases for other specifiers if needed
            default:
                *str++ = *p; // Default case for unknown specifier
                break;
            }
        }
        else
        {
            *str++ = *p; // Copy character as is
        }
        p++;
    }
    *str = '\0'; // Null-terminate the string
}

void Serial_printf(const char *format, ...)
{
    char buffer[256]; // Use small buffer to avoid stack issues

    va_list args;
    va_start(args, format);
    Serial_Printf_Char(buffer, format, args);
    va_end(args);

    HAL_UART_Transmit(&huart2, reinterpret_cast<uint8_t *>(buffer), strlen(buffer), UART_TIMEOUT);
}

HAL_StatusTypeDef Serial_Transmit_Receive(uint8_t *tx_buf, uint16_t tx_size, uint8_t *rx_buf, uint16_t rx_size)
{
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart2, tx_buf, tx_size, UART_TIMEOUT);
    if (status != HAL_OK)
    {
        return status;
    }
    return HAL_UART_Receive(&huart2, rx_buf, rx_size, UART_TIMEOUT);
}

HAL_StatusTypeDef Serial_transmit(uint8_t *tx_buf, uint16_t tx_size, uint16_t tx_timout)
{
    return HAL_UART_Transmit(&huart2, tx_buf, tx_size, tx_timout);
}

HAL_StatusTypeDef Serial_receive(uint8_t *rx_buf, uint16_t rx_size, uint16_t rx_timout)
{
    return HAL_UART_Receive(&huart2, rx_buf, rx_size, rx_timout);
}
