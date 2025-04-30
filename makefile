build:
	g++ -std=c++11 -o server server.cpp -Iinclude -Llib -lsfml-network -lsfml-system --static
	g++ -std=c++11 -o client client.cpp -Iinclude -Llib -lsfml-network -lsfml-system --static
	g++ -o server-player server-player.cpp -Iinclude -Llib -lsfml-network -lsfml-system --static
	g++ -o client-player client-player.cpp -Iinclude -Llib -lsfml-network -lsfml-system --static