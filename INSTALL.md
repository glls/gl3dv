# Installation

Tested on Windows 2010 with VS 2019 Express, 
sources used : [Boost 1.74,0](https://sourceforge.net/projects/boost/files/boost-binaries/), [Coin3D](https://github.com/coin3d/coin) and [SoWin](https://github.com/coin3d/sowin) from GitHub


## install Coin3D

cmake -Hcoin -Bcoin_build -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=T:\Projects\Coin -DBOOST_ROOT=G:\Projects\Libs\boost_1_74_0 -DCOIN_BUILD_DOCUMENTATION=OFF

cmake --build coin_build --target ALL_BUILD --config Release -- /nologo /verbosity:minimal /maxcpucount
cmake --build coin_build --target INSTALL --config Release -- /nologo /verbosity:minimal /maxcpucount


## install SoWin

cmake -Hsowin -Bsowin_build -G "Visual Studio 16 2019" -DCMAKE_INSTALL_PREFIX=T:\Projects\Coin -DBOOST_ROOT=G:\Projects\Libs\boost_1_74_0 -DCOIN_BUILD_DOCUMENTATION=OFF -DCMAKE_PREFIX_PATH=T:/Projects/Coin/lib/cmake/Coin-4.0.1/

cmake --build sowin_build --target ALL_BUILD --config Release -- /nologo /verbosity:minimal /maxcpucount
cmake --build sowin_build --target INSTALL --config Release -- /nologo /verbosity:minimal /maxcpucount
