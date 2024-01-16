NAME = apagon
DEPS = 

$(NAME): $(NAME).o $(DEPS)
	gcc -o $@ $(NAME).o 

test: test.o $(DEPS)
	gcc -o test test.o

%.o: %.c $(DEPS)
	gcc -c $< -g

.PHONY: clean
clean:
	rm -f $(NAME) *.o
	rm -f test *.o