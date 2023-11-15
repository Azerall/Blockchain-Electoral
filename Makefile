all: main1 main2 main3 main4 main5

main1: part1.o main1.o
	gcc part1.o main1.o -o main1

main2: part1.o part2.o main2.o
	gcc part1.o part2.o main2.o -o main2

main3: part1.o part2.o part3.o main3.o
	gcc part1.o part2.o part3.o main3.o -o main3

main4: part1.o part2.o part3.o part4.o main4.o
	gcc part1.o part2.o part3.o part4.o main4.o -o main4 -lssl -lcrypto

main5: part1.o part2.o part3.o part4.o part5.o main5.o
	gcc part1.o part2.o part3.o part4.o part5.o main5.o -o main5 -lssl -lcrypto

part1.o: part1.c part1.h
	gcc -c part1.c -o part1.o

part2.o: part2.c part1.h part2.h
	gcc -c part2.c -o part2.o

part3.o: part3.c part1.h part2.h part3.h
	gcc -c part3.c -o part3.o

part4.o: part4.c part1.h part2.h part3.h part4.h
	gcc -c part4.c -o part4.o -lssl -lcrypto

part5.o: part5.c part1.h part2.h part3.h part4.h part5.h
	gcc -c part5.c -o part5.o -lssl -lcrypto

main1.o: main1.c part1.h
	gcc -c main1.c -o main1.o

main2.o: main2.c part1.h part2.h
	gcc -c main2.c -o main2.o

main3.o: main3.c part1.h part2.h part3.h
	gcc -c main3.c -o main3.o

main4.o: main4.c part1.h part2.h part3.h part4.h
	gcc -c main4.c -o main4.o -lssl -lcrypto

main5.o: main5.c part1.h part2.h part3.h part4.h part5.h
	gcc -c main5.c -o main5.o -lssl -lcrypto

clean:
	rm -f *.o
	rm -f main1 main2 main3 main4 main5
