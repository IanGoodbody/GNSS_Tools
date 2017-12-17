parseNovAtel : parseNovAtel.o header.o decoderLibrary.o bestpos.o range.o  trackstat.o
	gcc -o parseNovAtel -Wall parseNovAtel.o header.o bestpos.o range.o trackstat.o decoderlibrary.o
	chmod +x parseNovAtel

test : parseNovAtel.o header.o decoderLibrary.o bestpos.o range.o trackstat.o
	gcc -o parseNovAtel -Wall parseNovAtel.o header.o decoderLibrary.o bestpos.o range.o trackstat.o
	chmod +x parseNovAtel
	./parseNovAtel col testLog.gps > testOut.txt
	less testOut.txt

parseNovAtel.o : parseNovAtel.h parseNovAtel.c
	gcc -c parseNovAtel.c

header.o : parseNovAtel.h header.c
	gcc -c header.c

decoderLibrary.o : parseNovAtel.h decoderLibrary.h decoderLibrary.c
	gcc -c decoderLibrary.c

bestpos.o : parseNovAtel.h bestpos.c
	gcc -c bestpos.c

range.o : parseNovAtel.h range.c
	gcc -c range.c

trackstat.o : parseNovAtel.h trackstat.c
	gcc -c trackstat.c

rawcnavframe.o : parseNovAtel.h rawcnavframe.c
	echo TODO: inegrate RAWCNAVFRAME into makefile

clean : 
	rm -vf *.o
	rm -vf *.csv
	rm -vf *.txt
	rm -f parseNovAtel
	rm -f parseNovAtel.mexa64
