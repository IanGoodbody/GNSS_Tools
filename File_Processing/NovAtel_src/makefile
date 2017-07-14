parseNovAtel : parseNovAtel.o header.o bestpos.o range.o rawcnavframe.o
	gcc -o parseNovAtel -Wall parseNovAtel.o header.o bestpos.o range.o
	chmod +x parseNovAtel

test : parseNovAtel.o header.o bestpos.o range.o rawcnavframe.o
	gcc -o parseNovAtel parseNovAtel.o header.o bestpos.o range.o
	chmod +x parseNovAtel
	./parseNovAtel testLog.gps > testOut.txt
	less testOut.txt

parseNovAtel.o : parseNovAtel.h parseNovAtel.c
	gcc -c parseNovAtel.c

header.o : parseNovAtel.h header.c
	gcc -c header.c

bestpos.o : parseNovAtel.h bestpos.c
	gcc -c bestpos.c

range.o : parseNovAtel.h range.c
	gcc -c range.c

rawcnavframe.o : parseNovAtel.h rawcnavframe.c
	echo TODO: inegrate RAWCNAVFRAME into makefile

clean : 
	rm -vf *.o
	rm -vf *.csv
	rm -vf *.txt
	rm -f parseNovAtel
	rm -f parseNovAtel.mexa64
