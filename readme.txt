To build using MS Visual Studio 9.0 Express:

1)
vcvars32

2)
To Install GLUT: see
http://www.xmission.com/~nate/glut.html

3) To build:
cl main.c landersim.c bitmappic.c -Ic:\inc\ c:\lib\glut32.lib


------------------------------------------------------
To build on Ubuntu (Hardy):


1) To Install GLUT:

sudo apt-get install libglut3-dev



2) To build:

gcc main.c bitmappic.c landersim.c -lglut -o lander


------------------------------------------------------
To build on Ubuntu 16.04 x86_64:

1) To install GLUT:
sudo apt-get install freeglut3

2) To build:
gcc main.c bitmappic.c landersim.c -lglut -lGL -lm -o lander

