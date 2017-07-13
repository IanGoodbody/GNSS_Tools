default : parseNovAtel.o header.o bestpos.o
	gcc -o parseNovAtel parseNovAtel.o header.o bestpos.o
	chmod +x parseNovAtel

parseNovAtel.o : parseNovAtel.h parseNovAtel.c
	gcc -c parseNovAtel.c

header.o : parseNovAtel.h header.c
	gcc -c header.c

bestpos.o : parseNovAtel.h bestpos.c
	gcc -c bestpos.c

clean : 
	rm -vf *.o
	rm -f parseNovAtel
	rm -f parseNovAtel.mexa64
