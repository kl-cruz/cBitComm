#include <stdio.h>
#include "cbitcomm.h"

void handler( uint8_t command_flag, uint8_t* data, uint16_t size )
{
    printf("Yeah handler!\n");
}

cbitcomm_command_t test = {
    0x01, &handler, NULL
};

static uint8_t test_array[] = {
    0xAA, 0x01, 0x02, 0x90, 0xA5, 0xa4, 0xfd
};

int main(void)
{
    uint32_t i = 0;
    printf("Hello World!\n");
    cbitcomm_init();

    cbitcomm_register_command(&test);
    for( i = 0; i < 7; i++)
    {
        cbitcomm_process_byte(test_array[i]);
    }
    return 0;
}

