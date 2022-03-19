NAME	=	proxy-server

SRC		=	main.cpp \
			Client/Client.cpp \
			Proxy/Proxy.cpp \
			Proxy/putToLogFile.cpp \

CC		=	g++ -std=c++17

FLAGS	=

OBJS	=	${SRC:.cpp=.o}

$(NAME): $(OBJS)
		$(CC) $(FLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

clean:
	rm -rf $(OBJS)

fclean: clean
	rm -rf $(NAME)

re:	fclean all

run: fclean all
	make clean && clear && ./$(NAME) 8080 123

.PHONY: all clean fclean re