all: mvdcsd iousbcap pongosh astdongle

mvdcsd:
	clang -I ../ mvdcsd.c ../mvusblib.c ../mvftdi.c -o mvdcsd -framework IOKit -framework CoreFoundation

iousbcap:
	clang -I ../ main.c ../mvusblib.c -o iousbcap -framework IOKit -framework CoreFoundation

pongosh:
	clang -I ../ pongo.c ../mvusblib.c -o pongosh -framework IOKit -framework CoreFoundation

astdongle:
	clang -I ../ astdongle.c ../mvusblib.c ../mvftdi.c -o astdongle -framework IOKit -framework CoreFoundation

clean:
	rm iousbcap
	rm mvdcsd
	rm pongosh
	rm astdongle
