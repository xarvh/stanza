
#	 -lpng

all:
	g++ st.cpp -o st -lGL -lm -lglut


run:
	./st `cat out.st`
