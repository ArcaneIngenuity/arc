gcc -I. -c arctyper.c 
gcc -I. -c ezxml/ezxml.c -DEZXML_NOMMAP 
gcc -o arctyper ezxml.o arctyper.o 