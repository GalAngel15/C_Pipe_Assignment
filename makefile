all: loopcmp nicecmp

	nicecmp: nicecmp.c
		gcc -o nicecmp nicecmp.c
	loopcmp: loopcmp.c
		gcc -o loopcmp loopcmp.c 
clean:
	rm loopcmp nicecmp
