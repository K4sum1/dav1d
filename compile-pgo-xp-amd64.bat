set CFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set CPPFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set CXXFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set LDFLAGS=/SUBSYSTEM:CONSOLE,5.2 /opt:REF,ICF
mkdir build-pgo-xp-amd64
meson setup --default-library=static --buildtype release -Db_pgo=generate build-pgo-xp-amd64
meson compile -C build-pgo-xp-amd64
build-pgo-xp-amd64/tools/dav1d.exe -i ../../Sparks-5994fps-AV1-10bit-1920x1080-film-grain-synthesis-2013kbps.obu --framethreads 16 --tilethreads 4 --muxer null
meson configure -Db_pgo=use build-pgo-xp-amd64
meson compile -C build-pgo-xp-amd64
