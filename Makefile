redmagic: redmagic.c

test: redmagic
		./test.sh

clean:
		rm -f redmagic *.o *~ tmp*

