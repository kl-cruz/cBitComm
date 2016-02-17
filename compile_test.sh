#!/bin/bash
gcc main.c cbitcomm.c crc16.c -DCBITCOMM_STARTBYTE=0xAA -o cbitcomm_test
