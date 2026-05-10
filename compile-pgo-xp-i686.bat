set CFLAGS=-D_WIN32_WINNT=0x0501 -O2 -Ob2ity- -GS- -GFAT -Qpar -Qfast_transcendentals
set CPPFLAGS=-D_WIN32_WINNT=0x0501 -O2 -Ob2ity- -GS- -GFAT -Qpar -Qfast_transcendentals
set CXXFLAGS=-D_WIN32_WINNT=0x0501 -O2 -Ob2ity- -GS- -GFAT -Qpar -Qfast_transcendentals
set LDFLAGS=/SUBSYSTEM:CONSOLE,5.1 /opt:REF,ICF /largeaddressaware
mkdir build-pgo-xp-i686
meson setup --default-library=static --buildtype release -Db_pgo=generate build-pgo-xp-i686
meson compile -C build-pgo-xp-i686
build-pgo-xp-i686/tools/dav1d.exe -i ../../Sparks-5994fps-AV1-10bit-1920x1080-film-grain-synthesis-2013kbps.obu --framethreads 16 --tilethreads 4 --muxer null
meson configure -Db_pgo=use build-pgo-xp-i686
meson compile -C build-pgo-xp-i686
