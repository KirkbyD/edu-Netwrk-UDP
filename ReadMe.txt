AmethystEngine by:
	Dylan Kirkby,
	Brian Cowan,
	Ivan Parkhomenko


Networking Final Project (3)


Created in Visual Studio Community 2019
Runs in x64 only, Release recommended for performance.

Client project in \AmethystEngine
Server project in \AmethystUDPServer
assets and configuration can be found in each projects respective /data folder


To run, Right click the server in visual studio and select debug -> stat new instance
Do the same for each additional client.

Client side code can be found in AmethystEngine (by filter in visual studio)
	UDP Client
		cUDPClient.hpp
		cUDPClient.cpp
	SourceFiles
		Main.cpp		Creation and updates
	UserInput
		cKeyBindings.cpp
		cMouseBindings.cpp

Server side code can be found in AmethystUDPServer (by filter in visual studio)
	UDP Server
		cUDPServer.hpp
		cUDPServer.cpp
	SourceFiles
		Main.cpp		Creation and updates
	Physics
		cPhysicsManager.cpp	TestForNetworkBulletCollisions()

Controls:
Client
Mouse:
	Move		Adjust camera viewing angle
			Also Rotate objects Yaw/Pitch
	Click		Shoot Fireball

Keyboard:	
	A		Increase left velocity relative to camera
	D		Increase right velocity relative to camera
	W		Increase forward velocity relative to camera
	S		Increase backward velocity relative to camera
	SPACE		Reduce velocity 50% (hold to stop)	
	
	P		Birdseye camera
	[		Object/player view
	]		Toggle extra objects (server location, dead reckoning, client prediction)

	ESC		Exit program.

Server:
	ESC		Exit program.