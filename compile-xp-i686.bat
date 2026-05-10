set CFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity- -GS- -GFAT -Qpar -Qfast_transcendentals
set CPPFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity- -GS- -GFAT -Qpar -Qfast_transcendentals
set CXXFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity- -GS- -GFAT -Qpar -Qfast_transcendentals
set LDFLAGS=/SUBSYSTEM:CONSOLE,5.1 /opt:REF,ICF /largeaddressaware
mkdir build-xp-i686
meson setup --default-library=static --buildtype release -Denable_tests=false build-xp-i686
meson compile -C build-xp-i686
