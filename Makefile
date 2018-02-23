all:
	mkdir -p bin
	g++ -o bin/PCPal main.cpp ScreenOverlay.cpp -std=c++14 -lX11 -lXfixes -lXcomposite `pkg-config --libs --cflags opencv` -lcairo -I/usr/include/cairo  -g

clean:
	rm -r bin