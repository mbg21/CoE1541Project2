#!/bin/bash
rm ../build/testad
gcc -w -o ../build/testad TestAddressDecoder.c -U AD_DEBUG -D AD_DEBUG=1
chmod +x ../build/testad
../build/testad
