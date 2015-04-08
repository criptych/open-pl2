**See current [Progress](Progress.md)**

# Overview #
This is an attempt at writing an open source engine and toolset for the somewhat popular PolygonLove2 (PL2, ポリゴンラブ２) Japanese 3D dating sim game made by Purple Heart.

No resource files will be provided here, just the engine. The best part of a dating sim game is the story and characters, so go buy the original!

However tools to create new resource files and edit existing ones will be made available here when the formats have been fully reversed.

If you have any suggestion or comment, feel free to make an 'issue' or email me (LightDark3D 

&lt;at&gt;

 gmail 

&lt;dot&gt;

 com). Also feel free to just say hi and that you're interested in the project. The more people I know are interested, the more I'll want to work on it.

All user interface use QT4, this should run on Windows, Mactels and Unix machines.

All 3D code uses [OpenGL](http://www.opengl.org/) and [Cg](http://developer.nvidia.com/page/cg_main.html).

**Please note:** PolygonLove2 is an '18' rated game, and does contain explicit content. This project page will not contain any such content, or link to it.



# Library #
The library (reads files, etc) is written in C99 and can be used with Windows, UNIX and Mac systems. The only dependency it has at the moment is iconv, which can be disabled.

The PL2 format library doesn't support big-endian machines (though this shouldn't be hard to change).

Currently the library can not open compressed PL2 files, since the compression system is still being reversed.
(The 'PakPl2.exe' tool creates uncompressed PL2 files, however the files which come with the game are compressed. You can use MltPl2.exe to unpack the PL2 files and then repack them with PakPl2.exe to create compatible files.)



# Viewer #
The viewer at the moment supports loading and displaying of TMB model files using the format library. There is no animation support yet.



# TODO #
In order of priroty:
  1. PL2 Decompression support
  1. TSB reverse/finish TMB reverse
  1. Bone palette generation (model seem to use around 200 bones, but shader only have about 30 loaded at a time)
  1. Animation
  1. Clean up 3D viewer and upload
  1. Various import/export tools for file formats
  1. PSD import support (Why they use PSD?)
  1. Extended format and feature creation (see below)
  1. Script support for story mode
I'd also like to setup a website somewhere suitible hosting user created models and modifications. Please e-mail me if you have suitible hosting.



# Super-PL2 #
After I have the basic PL2 format support done, I plan to add new features that might be fun to code. These will really be just so I can mess around.
  1. New mesh/animation formats supporting more stuff
  1. Liquid simulation
  1. Cloth and hair dynamic
  1. HQ screenshot/video-capture support (using global illumination, etc)
  1. Fun fragment shaders (cel shading, fur?, etc)



# Q&A #

Q) Why is the format library in C, but the rest of the project in C++?
A) I prefer to have libraries and such as C since then I (and other people) can use them in non-C++ projects MUCH easier.

Q) Why QT4?
A) It's a nice multiplatform GUI that works well on the different platforms. I do not think GTK works well on non-unix platforms, and I don't think .NET works well on non-windows platforms.

Q) Who are you? And why work on this project.
A) A game developer currently working in Japan. It's mainly so I can mess around with a relativly simple game system and add some fun features.