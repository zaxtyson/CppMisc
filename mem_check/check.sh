#!/bin/bash

gcc leak.c -o leak
valgrind --track-fds=yes --leak-check=full --show-leak-kinds=all --undef-value-errors=yes ./leak