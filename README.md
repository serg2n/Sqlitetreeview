# Sqlitetreeview

Sqlite data base and geopackage files browser data and BLOB viewer Viewing sqlite database files, as well as geodata files in gpkg format. Detailed viewing of byte data fields (BLOB).

# Building a program

The library is used as a basis for the program [Win32++](https://win32-framework.sourceforge.net),  and also a library [sqlite3]([GitHub - sqlite/sqlite: Official Git mirror of the SQLite source tree](https://github.com/sqlite/sqlite)). Static linking is used when building the program. The sqlite library is built statically using [vcpkg]([GitHub - microsoft/vcpkg: C++ Library Manager for Windows, Linux, and MacOS](https://github.com/microsoft/vcpkg)). Both libraries used are included in the project repository. 

         All project dependencies are included in the executable file of the program, so the program does not use any additional libraries. The program has been tested on Windows 10 and Windows 7 (on [VirtualBox](https://www.virtualbox.org)). Work with large data tables and large BLOB data was checked.
