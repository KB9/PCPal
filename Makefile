all:
	mkdir -p bin
	g++ -o bin/PCPal main.cpp ScreenOverlay.cpp KeyboardSimulator.cpp -std=c++14 -lX11 -lXext -lXfixes -lXcomposite `pkg-config --libs --cflags opencv` -g

clean:
	rm -r bin