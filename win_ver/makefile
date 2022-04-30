CFLAGS=/W4 /EHsc /std:c++17 /Iinclude /utf-8 /nologo

LIB_NAMES= driver
LIBS=$(patsubst %,obj\\%.obj,$(LIB_NAMES))

all: dirs main

main: src\main.cpp $(LIBS)
    cl $(CFLAGS) /Fo:obj\ /Fe:main.exe $?

{src\}.cpp{obj\}.obj::
    cl $(CFLAGS) /c /Fo:obj\ $<

# obj\utility.obj::            src\$(@B).cpp include\$(@B).h
#     cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp

# obj\disk_explorer.obj::      src\$(@B).cpp include\$(@B).h
#     cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp

# obj\device.obj::             src\$(@B).cpp include\$(@B).h
#     cl $(CFLAGS) /c /Fo:obj\ src\$(@B).cpp

dirs:
    (IF NOT EXIST obj (MKDIR obj))

clean:
    FOR %I IN (obj\*) DO @((echo Removing file %I) & (del %I))
    IF EXIST obj (rmdir obj)
.PHONY: clean