# ArmyChess

This is my project (#1) for the summer semester (30240522-3) in Tsinghua University. Mainly focused on Qt and Internet programming.

## Build

`MinGW 8.1.0` (your c++ compiler) and `Qt6` is required, and `Qt 6.1.2` is recommended 
(because it is used for development, and I haven't tested it on any other versions).

This program supports both `cmake` and `qmake`.

#### Using cmake

You need to specify your `CMAKE_PREFIX_PATH` in `CMakeLists.txt` to the `cmake` folder provided
by Qt. It should look like `QT_INSTALL_FOLDER/QT_VERSION/mingw81_64/lib/cmake`.

```
> cmake -S . -B build -G "MinGW Makefiles"
> cmake --build build
```

#### Using qmake

```
> qmake
> make
```

## Develop Progress

#### Interface

- [x] main window & menu bar - 2021/08/16
- [x] board & chess - 2021/08/17
- [x] board synchronization - 2021/08/19

#### Connection

- [x] connection - 2021/08/19
- [x] disconnection - 2021/08/19

#### Rules

- [x] init board and chess - 2021/08/17
- [x] flip chess - 2021/08/17
- [x] move chess - 2021/08/20
- [x] eat chess - 2021/08/20

#### Victory

- [x] judge victory - 2021/08/22
- [x] timeout - 2021/08/22
- [x] pop-up window - 2021/08/22
