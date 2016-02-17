#include "cbitcomm.h"
#include "crc16.h"
#include <stdlib.h>

#ifndef CBITCOMM_STARTBYTE
#error "Define start byte!"
#endif

enum cbitcomm_state {
    CBITCOMM_FIND_START_BYTE = 0x00,
    CBITCOMM_FIND_CMD_BYTE,
    CBITCOMM_FIND_PAYLOAD_LEN,
    CBITCOMM_COLLECT_PAYLOAD,
    CBITCOMM_CONTROL_CHECKSUM,
    CBITCOMM_EXECUTE_COMMAND,
    CBITCOMM_SEND_BACK_DATA,
};

static cbitcomm_command_t *commandsList;
static uint8_t payload_buffer[256];
static uint32_t current_state;
static uint8_t payload_len;
static uint8_t payload_idx;
static uint16_t payload_crc;
static uint16_t fetched_crc;
static uint8_t checksum_idx;
static uint8_t command_id;

static void cbitcomm_change_state(uint32_t state)
{
    current_state = state;
}


static uint32_t cbitcomm_find_command(uint32_t command_id)
{
    uint32_t command_counter = 0;
    cbitcomm_command_t *command_iterator = commandsList;

    if(commandsList == NULL)
    {
        return command_counter;
    } else {

        do {

            if(command_iterator->command_id == command_id)
            {
                command_counter++;
            }

            command_iterator = command_iterator->next;
        } while(command_iterator != NULL);

    }

    return command_counter;
}

static void cbitcomm_exec_command(uint32_t command_id)
{
    cbitcomm_command_t *command_iterator = commandsList;

    if(commandsList == NULL)
    {
        return;
    } else {

        do {

            if(command_iterator->command_id == command_id)
            {
                command_iterator->handler(0, payload_buffer, payload_len);
            }

            command_iterator = command_iterator->next;
        } while(command_iterator != NULL);

    }

}

/**
 * @brief Register command structure with handler
 * @param new_command
 */
void cbitcomm_register_command(cbitcomm_command_t *new_command)
{
    cbitcomm_command_t *command = commandsList;

    if(commandsList == NULL)
    {
        commandsList = new_command;
    } else {
        while(command->next != NULL)
        {
            command = command->next;
        }
        command->next = new_command;
    }
}

/**
 * @brief Structures and command list initialization
 */
void cbitcomm_init(void)
{
    commandsList = NULL;
    current_state = CBITCOMM_FIND_START_BYTE;
}

/**
 * @brief Process byte machine state
 * @param byte Processing byte
 */
void cbitcomm_process_byte(uint8_t byte)
{

    switch (current_state) {
    case CBITCOMM_FIND_START_BYTE:

        if( byte == CBITCOMM_STARTBYTE ) {
            cbitcomm_change_state(CBITCOMM_FIND_CMD_BYTE);
            payload_len = 0;
            command_id = 0;
        }

        break;
    case CBITCOMM_FIND_CMD_BYTE:

        if(cbitcomm_find_command( byte ) > 0)
        {
            cbitcomm_change_state(CBITCOMM_FIND_PAYLOAD_LEN);
            command_id = byte;
        } else {
            cbitcomm_change_state(CBITCOMM_FIND_START_BYTE);
        }

        break;
    case CBITCOMM_FIND_PAYLOAD_LEN:
        payload_len = byte;
        payload_idx = 0;
        payload_crc = CRC16_INIT_VALUE;
        fetched_crc = 0;
        cbitcomm_change_state(CBITCOMM_COLLECT_PAYLOAD);
        break;
    case CBITCOMM_COLLECT_PAYLOAD:
        payload_buffer[payload_idx] = byte;
        payload_crc = crc16(byte, payload_crc);
        payload_idx++;
        if(payload_len == payload_idx)
        {
            cbitcomm_change_state(CBITCOMM_CONTROL_CHECKSUM);
            checksum_idx = 0;
        }
        break;
    case CBITCOMM_CONTROL_CHECKSUM:
        fetched_crc |= byte << checksum_idx;
        checksum_idx++;

        if(checksum_idx == 2)
        {

            if(payload_crc == fetched_crc)
            {
                cbitcomm_change_state(CBITCOMM_EXECUTE_COMMAND);
            } else
            {
                cbitcomm_change_state(CBITCOMM_FIND_START_BYTE);
            }

        } else {
            break;
        }
    case CBITCOMM_EXECUTE_COMMAND:
        cbitcomm_exec_command(command_id);
    case CBITCOMM_SEND_BACK_DATA:
        break;
    default:
        cbitcomm_change_state(CBITCOMM_FIND_START_BYTE);
    }
}
