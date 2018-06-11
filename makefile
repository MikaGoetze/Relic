PLATFORM := $(shell uname)

# Use pkg-config to get the right libraries for your platform
GL_LIBS = `pkg-config --static --libs glfw3` -lGLEW -lGL 
EXT = 
DEFS = `pkg-config --cflags glfw3` -I.

# Any other platform specific libraries here...
ifneq (, $(findstring CYGWIN, $(PLATFORM)))
    GL_LIBS = -lopengl32 -lglfw3 -lglew32
	EXT = .exe
    DEFS =-DWIN32 -I.
endif

LINK += Shader.o Camera.o DirectionalLight.o FPSCamera.o Light.o PointLight.o SpotLight.o Texture.o OrbitCamera.o Input.o Model.o
LINK += HDRTexture.o MeshRenderer.o GameObject.o Relic.o RenderUtil.o Scene.o Serializable.o Serializer.o Transform.o Util.o Window.o
LINK += PlayerMove.o Test.o user_main.o

.PHONY:  clean

all : assign3

assign3: main.o $(LINK)
	g++ --std=c++11 -I. $(DEFS) -o assign3 $(LINK) $(GL_LIBS) main.o 

main.o: Core/main.cpp $(LINK)
	g++ --std=c++11 -I. $(DEFS) -c Core/main.cpp

Shader.o: Lighting/Scripts/Shader.cpp Lighting/Scripts/Shader.h
	g++ --std=c++11 -I. $(DEFS) -c Lighting/Scripts/Shader.cpp

Camera.o: Camera/Camera.cpp Camera/Camera.h
	g++ --std=c++11 -I. $(DEFS) -c Camera/Camera.cpp

DirectionalLight.o: Lighting/Scripts/DirectionalLight.cpp Lighting/Scripts/DirectionalLight.h
	g++ --std=c++11 -I. $(DEFS) -c Lighting/Scripts/DirectionalLight.cpp

FPSCamera.o: Camera/FPSCamera.cpp Camera/FPSCamera.h
	g++ --std=c++11 -I. $(DEFS) -c Camera/FPSCamera.cpp

Input.o: Core/Input.cpp Core/Input.h
	g++ --std=c++11 -I. $(DEFS) -c Core/Input.cpp

Model.o: Model/Model.cpp Model/Model.h
	g++ --std=c++11 -I. $(DEFS) -c Model/Model.cpp

Light.o: Lighting/Scripts/Light.cpp Lighting/Scripts/Light.h
	g++ --std=c++11 -I. $(DEFS) -c Lighting/Scripts/Light.cpp

PointLight.o: Lighting/Scripts/PointLight.cpp Lighting/Scripts/PointLight.h
	g++ --std=c++11 -I. $(DEFS) -c Lighting/Scripts/PointLight.cpp

SpotLight.o: Lighting/Scripts/SpotLight.cpp Lighting/Scripts/SpotLight.h
	g++ --std=c++11 -I. $(DEFS) -c Lighting/Scripts/SpotLight.cpp

Texture.o: Texturing/Texture.cpp Texturing/Texture.h
	g++ --std=c++11 -I. $(DEFS) -c Texturing/Texture.cpp
	
HDRTexture.o: Texturing/HDRTexture.h Texturing/HDRTexture.cpp:
	g++ --std=c++11 -I. $(DEFS) -c Texturing/HDRTexture.cpp

MeshRenderer.o: Model/MeshRenderer.h Model/MeshRenderer.cpp:
	g++ --std=c++11 -I. $(DEFS) -c Model/MeshRenderer.cpp

GameObject.o: Core/GameObject.h Core/GameObject.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/GameObject.cpp

Relic.o: Core/Relic.h Core/Relic.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/Relic.cpp
	
RenderUtil.o: Core/RenderUtil.h Core/RenderUtil.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/RenderUtil.cpp

Scene.o: Core/Scene.h Core/Scene.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/Scene.cpp
	
Serializable.o: Core/Serializable.h Core/Serializable.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/Serializable.cpp

Serializer.o: Core/Serializer.h Core/Serializer.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/Serializer.cpp
	
Transform.o: Core/Transform.h Core/Transform.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/Transform.cpp
	
Util.o: Core/Util.h Core/Util.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/Util.cpp
	
Window.o: Core/Window.h Core/Window.cpp
	g++ --std=c++11 -I. $(DEFS) -c Core/Window.cpp
	
PlayerMove.o: PlayerMove.h PlayerMove.cpp
	g++ --std=c++11 -I. $(DEFS) -c PlayerMove.cpp
	
Test.o: Test.h Test.cpp:
	g++ --std=c++11 -I. $(DEFS) -c Test.cpp
	
user_main.o: user_main.cpp:
	g++ --std=c++11 -I. $(DEFS) -c user_main.cpp

clean:
	rm -f *.o run$(EXT)
