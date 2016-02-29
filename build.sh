#!/bin/bash

cd serversrc
make clean
make
cp server ../

cd ..

cd clientsrc
make clean
make
cp client ../

cd ..