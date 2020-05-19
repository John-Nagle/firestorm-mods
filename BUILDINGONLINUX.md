# Firestorm build procedure 2020, for Firestorm 6.3.8

**Unofficial**

John Nagle

Animats Resident

March 12, 2020

## Introduction
Building Firestorm on Linux is easy.
Except that there are no current instructions on how to do it.

Several set of instructions are available.

- http://wiki.phoenixviewer.com/fs_compiling_firestorm_64bit_ubuntu

- https://vcs.firestormviewer.org/phoenix-firestorm-lgpl/files/5e2050564bd3730f0ce7843860afdb25da3d2346/README_BUILD_FIRESTORM_LINUX.txt

They are obsolete and contradict each other. After discussion with various Firestorm builders,
and combining their advice, I was able to build Firestorm on Ubuntu 18.04 LTS quite simply.
Here's how I did it.

## Fetching sources
Start with an empty directory.

    git clone https://vcs.firestormviewer.org/phoenix-firestorm
    git clone https://vcs.firestormviewer.org/autobuild-1.1
    git clone https://vcs.firestormviewer.org/fs-build-variables
    
Everything is under "git" source control now. No more Mercurial.

## Installing prerequsite software

Most of what's needed is already in Ubuntu 18.04 LTS. In particular,
the standard GCC compiler, gcc 7.5.0, will work. No need to install
some antique GCC compiler any more.  Just install
  
    pip install llbase
    sudo apt-get install cmake
    sudo apt-get libxinerama-dev
   
Surprisingly, that's all that was needed. It's possible that something
else may be needed on some other system, but if something is missing,
the build will fail and it should be obvious what needs to be installed.

## Configuring

    export AUTOBUILD_VARIABLES_FILE=`readlink -f fs-build-variables/variables`
    cd phoenix-firestorm
    ../autobuild-1.1/bin/autobuild configure -A64 -c ReleaseFS_open
    
The configure step took hours, because it was downloading source files from a very slow server.
That may have been a temporary problem with the server.
If it's so slow you think it has failed, you can kill it and run it as

    ../autobuild-1.1/bin/autobuild configure --debug -A64 -c ReleaseFS_open 
    
which will produce progress messages for all the file downloads. 

## Compiling

     nice ../autobuild-1.1/bin/autobuild build -A64 -c ReleaseFS_open
     
This takes 1-2 hours and uses all available CPUs.
The slow configuring and build steps can be restarted if needed.
They pick up from where they left off.
     
## Running

    cd /build-linux-x86_64/newview/packaged
    ./firestorm
    
Worked the first time. Even voice worked.

## Build warnings

### Incompatibiilty with system version of "strip"?

    strip:/home/john/projects/firestorm/6.3.8/phoenix-firestorm/build-linux-x86_64/newview/packaged/bin/win32/ca-bundle.crt: File format not recognized
    strip:/home/john/projects/firestorm/6.3.8/phoenix-firestorm/build-linux-x86_64/newview/packaged/bin/ca-bundle.crt: File format not recognized
    
### Attempt to perform privileged function in nonprivileged build

    Running command: sudo -n chown root:root /home/john/projects/firestorm/6.3.8/phoenix-firestorm/build-linux-x86_64/newview/packaged/bin/chrome-sandbox || exit 0
    sudo: a password is required
    Running command: sudo -n chmod 4755 /home/john/projects/firestorm/6.3.8/phoenix-firestorm/build-linux-x86_64/newview/packaged/bin/chrome-sandbox || exit 0
    sudo: a password is required
    
This probably has something to do with running the internal browser in sandbox mode.

## Building under Ubuntu 20.04

Notes from Marissa Orlov on 2020-05-18:

    [15:56] Marissa Orlova (marissaorloff): 20.04 is vastly different from that
    [15:56] Marissa Orlova (marissaorloff): not vastly
    [15:56] Marissa Orlova (marissaorloff): you have to specify python2 and make alternatives
    [15:57] Marissa Orlova (marissaorloff): no python-pip anymore
    [15:57] Marissa Orlova (marissaorloff): ogg_vorbis needs to be re-compiled or it won't link
    [15:58] Marissa Orlova (marissaorloff): But, you can download pip from the main site.. 
    but it defaults to .local so you can just make a $PATH
    [15:59] Marissa Orlova (marissaorloff): I started building in 20.04 a month before release 
    because I knew it was going to be messed up with python2 being handed its hat and shown the door
    [16.01] Marissa Orlova (marissaorloff): Other than that it's fine :)
    [16:01] Marissa Orlova (marissaorloff): gcc 9.3 works great for FS



