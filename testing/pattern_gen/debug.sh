#!/bin/bash

valgrind --leak-check=full --show-leak-kinds=all ./main 3 100
