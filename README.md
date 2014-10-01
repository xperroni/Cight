Cight
======

Cight is an implementation of a visual algorithm for teach-replay navigation of mobile robots. See [this article](https://github.com/xperroni/Yamabiko/blob/master/2014-1/Article/Mobile%20Robot%20Path%20Drift%20Estimation%20using%20Visual%20Streams.pdf) and the [Dejavu](https://github.com/xperroni/Dejavu) demo application for reference.

Cight is developed in C++ 03 on top of [boost](http://www.boost.org/) and [OpenCV](http://opencv.org/). A set of [cmake](http://www.cmake.org/) scripts is provided for building.

Building
--------

Open a command prompt, move into either folder `build/release/` or `build/debug/` (depending on which build mode you want to use) and enter the following commands:

    $ cmake -DCMAKE_BUILD_TYPE=$MODE ../..
    $ make -j2

Where `$MODE` should be either `Release` or `Debug`.

Alternatively, under Bash-compatible environments you can just call the `build.sh` script:

    $ ./build.sh

The build scripts expect to find the include files for [Clarus](https://github.com/xperroni/Clarus) on a folder at the same level as the project's base folder, for example:

    .
    |
    +--+ Cight
    |  |
    |  +-- build
    |  +-- include
    |  +-- src
    |
    +-- Clarus
       |
       +-- build
       +-- include
       +-- src

TO DO
-----

* Increment the cmake scripts with better dependency detection;
* Document the API and source files;
* Move the code base to C++ 11, when support is widespread enough.
