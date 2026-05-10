set CFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set CPPFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set CXXFLAGS=-D_WIN32_WINNT=0x0502 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set LDFLAGS=/SUBSYSTEM:CONSOLE,5.2 /opt:REF,ICF
mkdir build-xp-amd64
meson setup --default-library=static --buildtype release -Denable_tests=false build-xp-amd64
meson compile -C build-xp-amd64
