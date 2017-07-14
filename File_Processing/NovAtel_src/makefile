default : parseNovAtel.o header.o bestpos.o range.o
	gcc -o parseNovAtel -Wall parseNovAtel.o header.o bestpos.o range.o
	chmod +x parseNovAtel

test : parseNovAtel.o header.o bestpos.o range.o
	gcc -o parseNovAtel parseNovAtel.o header.o bestpos.o range.o
	chmod +x parseNovAtel
	./parseNovAtel col testLog.gps

parseNovAtel.o : parseNovAtel.h parseNovAtel.c
	gcc -c parseNovAtel.c

header.o : parseNovAtel.h header.c
	gcc -c header.c

bestpos.o : parseNovAtel.h bestpos.c
	gcc -c bestpos.c

range.o : parseNovAtel.h range.c
	gcc -c range.c

clean : 
	rm -vf *.o
	rm -vf *.dat
	rm -vf *.txt
	rm -vf *.csv
	rm -f parseNovAtel
	rm -f parseNovAtel.mexa64
