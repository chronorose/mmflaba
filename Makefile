all:
	touch main
	rm main
	gcc -fsanitize=address main.c -o main
	./main < test
