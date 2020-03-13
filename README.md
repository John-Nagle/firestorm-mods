# firestorm-mods

Temporary repository for some modifications to the Firestorm virtual world viewer

The sources for Firestorm are maintained elsewhere.  This is temporary storage for some fixes under
development. The changes are intended to work around some problems in the simulator by adding some sanity checks
to the viewer for motion that crosses sim boundaries. In time, these should be merged back into the
mainstream. The first version of these changes went into Firestorm in late 2018, and a new version is 
under development in 2020. 

# Build procedure

For Ubuntu 16.04 LTS:

See http://wiki.phoenixviewer.com/fs_compiling_firestorm_64bit_ubuntu

Those instructions almost work. But it's necessary to add the two "export"
statements below to force the use of GCC 4.8.  GCC 5.x will not work because
the binaries are incompatible with some pre-compiled libraries provided.
The only problem seems to be in a Boost function for some Perl-type regular
expression matcher.

Copy the files in this repository into 

    phoenix-firestorm-lgpl/indra/newview
    
Run:

    export CC=/usr/bin/gcc-4.8
    export CXX=/usr/bin/g++-4.8
    autobuild -m64 configure -c ReleaseFS_open -- --clean -DLL_TESTS:BOOL=FALSE
    autobuild -m64 build -c ReleaseFS_open

For Ubuntu 18.04 LTS, see [here](./BUILDINGONLINUX.md)
