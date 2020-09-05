all: cat checkcpupercentage checkresidentmemory executeCommands grep listFiles sortFile myShell

cat: cat.o
	gcc -o cat cat.o
	rm -f cat.o

cat.o: cat.c
	gcc -c -o cat.o cat.c

checkcpupercentage: checkcpupercentage.o charToInteger.o
	gcc -o checkcpupercentage checkcpupercentage.o charToInteger.o
	rm -f checkcpupercentage.o
	rm -f charToInteger.o

checkcpupercentage.o: checkcpupercentage.c
	gcc -c -o checkcpupercentage.o checkcpupercentage.c

charToInteger.o: charToInteger.c
	gcc -c -o charToInteger.o charToInteger.c
	
checkresidentmemory: checkresidentmemory.o
	gcc -o checkresidentmemory checkresidentmemory.o
	rm -f checkresidentmemory.o

checkresidentmemory.o: checkresidentmemory.c
	gcc -c -o checkresidentmemory.o checkresidentmemory.c

executeCommands: executeCommands.o
	gcc -o executeCommands executeCommands.o
	rm -f executeCommands.o

executeCommands.o: executeCommands.c
	gcc -c -o executeCommands.o executeCommands.c

grep: grep.o
	gcc -o grep grep.o
	rm -f grep.o

grep.o: grep.c
	gcc -c -o grep.o grep.c

listFiles: listFiles.o
	gcc -o listFiles listFiles.o
	rm -f listFiles.o

listFiles.o: listFiles.c
	gcc -c -o listFiles.o listFiles.c

sortFile: sortFile.o
	gcc -o sortFile sortFile.o
	rm -f sortFile.o

sortFile.o: sortFile.c
	gcc -c -o sortFile.o sortFile.c

myShell: myShell.o
	gcc -o myShell myShell.o
	rm -f myShell.o
	
myShell.o: myShell.c
	gcc -c -o myShell.o myShell.c