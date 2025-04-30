build:
	g++ -o server-player server-player.cpp -Iinclude -Llib -lsfml-network -lsfml-system --static
