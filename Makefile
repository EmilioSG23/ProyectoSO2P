NAME = apagon
DEPS = central.h
OBJECTS = central.o lluvia.o

$(NAME): $(NAME).o $(OBJECTS) $(DEPS)
	gcc -o $@ $(NAME).o $(OBJECTS) -lpthread

test: test.o $(DEPS)
	gcc -o test test.o

%.o: %.c $(DEPS)
	gcc -c $< -g

.PHONY: clean
clean:
	rm -f $(NAME) *.o
	rm -f test *.o