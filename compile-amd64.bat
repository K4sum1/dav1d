set CFLAGS=-D_WIN32_WINNT=0x0600 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set CPPFLAGS=-D_WIN32_WINNT=0x0600 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set CXXFLAGS=-D_WIN32_WINNT=0x0600 -O2 -Ob2ity -GS- -GFAT -Qpar -Qfast_transcendentals
set LDFLAGS=/SUBSYSTEM:CONSOLE,6.0 /opt:REF,ICF
mkdir build-amd64
meson setup --default-library=static --buildtype release -Denable_tests=false build-amd64
meson compile -C build-amd64
