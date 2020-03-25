all: hw1.c
	gcc hw1.c -o hw1
	@echo "Applying permissions..."
	chown root:root Makefile hw1
	chmod 544 Makefile
	chmod 555 hw1
	chmod u+s hw1
