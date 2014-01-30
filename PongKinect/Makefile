NAME = kinect
SRCS = 	main.cpp		\
	CamStreamer.cpp		\
	BlobDetector.cpp	\
	ConvexHullManager.cpp	\
	Pong.cpp

INCLUDE = -I/usr/include/opencv/

OBJS = $(SRCS:.cpp=.o) 

$(NAME) :
	g++ -I/usr/include/opencv-2.3.1/opencv/ -I/usr/include/opencv-2.3.1/ -lSDL -lSDL_image -lSDL_ttf -lopencv_highgui  -lfreenect -lfreenect_sync -W -Wall -Wextra $(SRCS) -o $(NAME)

all : $(NAME)

clean :
	rm -f $(OBJS)
	rm -f *~

fclean	:	clean
		rm -f $(NAME)

re	:	fclean all