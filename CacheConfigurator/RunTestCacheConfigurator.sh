#!/bin/bash
rm ../build/testcc
gcc TestCacheConfigurator.c -w -U CC_DEBUG -D CC_DEBUG=1 -o ../build/testcc 
chmod +x ../build/testcc
../build/testcc
