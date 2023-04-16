@echo off

echo Updating submodules

cd "Proximity\PrxEngine\External\yaml-cpp\"
git submodule update --recursive

echo Building yaml-cpp

mkdir build
cd build

echo Added [yaml-cpp] 'build' directory

echo Using CMAKE to build [yaml-cpp]

cmake ..

pause

