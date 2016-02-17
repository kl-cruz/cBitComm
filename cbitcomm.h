#ifndef CBITCOMM_H
#define CBITCOMM_H

#include <stdint.h>

typedef void (*cbitcomm_command_handler_t)( uint8_t command_flag, uint8_t* data, uint16_t size );

typedef struct cbitcomm_command {
    uint8_t command_id;
    cbitcomm_command_handler_t handler;
    struct cbitcomm_command *next;
} cbitcomm_command_t;


void cbitcomm_init(void);
void cbitcomm_register_command(cbitcomm_command_t *new_command);
void cbitcomm_process_byte(uint8_t byte);


#endif // CBITCOMM_H

