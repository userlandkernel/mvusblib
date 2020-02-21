INCLUDE_DIRS=-I ../ -I ../../
MVUSBLIB_SRC=../mvusblib.c ../../vendors.c ../../products.c ../../apple.c ../../futuretechdev.c
MVUSBLIB_LDFLAGS=-framework IOKit -framework CoreFoundation
MVFTDILIB_SRC=../mvftdi.c

all: builddir mvdcsd iousbcap pongosh astdongle

builddir:
	mkdir -p build/

mvdcsd:
	clang $(INCLUDE_DIRS) mvdcsd.c $(MVUSBLIB_SRC) $(MVFTDILIB_SRC) -o build/mvdcsd $(MVUSBLIB_LDFLAGS)

iousbcap:
	clang $(INCLUDE_DIRS) iousbcap.c $(MVUSBLIB_SRC) -o build/iousbcap $(MVUSBLIB_LDFLAGS)

pongosh:
	clang $(INCLUDE_DIRS) pongo.c $(MVUSBLIB_SRC) -o build/pongosh $(MVUSBLIB_LDFLAGS)

astdongle:
	clang $(INCLUDE_DIRS) astdongle.c $(MVUSBLIB_SRC) $(MVFTDILIB_SRC) -o build/astdongle $(MVUSBLIB_LDFLAGS)

clean:
	rm -rf build/
