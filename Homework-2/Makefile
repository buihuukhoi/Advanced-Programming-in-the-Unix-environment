all:
	gcc -o sandbox.so -shared -fPIC sharedLibrary.c -ldl
	gcc sandbox.c -o sandbox
	@echo "Applying permissions..."
	chmod 555 Makefile
	