# 2007/02/12 #

I've been busy at work recently, we're really behind on a game so I haven't had much time for personal projects (or sleep.... or being outside my office).

Animation now works, it has a couple of odd bugs (normals are not calculated perfectly, some meshes don't deform correctly) but at least it's almost there. All the skinning is currently performed on the CPU, which is a very slow way of doing things. To move to GPU skinning I need to split the meshes up into bone palettes, which will take some thought.

Once I get the bugs fixed and get skinning moved to the GPU I'll release a basic program. (Source code and win32 binaries)

I still can't reverse PL2 file compression. I am close, but just can't get over the final hurdle. It might be a while before there's support for this. (Unless somebody else wants to have a go, I have posted a few requests on a few boards)

Anyway, here's a screenshot of my QT4 debug app.

![http://open-pl2.googlecode.com/files/OpenPL2-2007-02-12.jpg](http://open-pl2.googlecode.com/files/OpenPL2-2007-02-12.jpg)