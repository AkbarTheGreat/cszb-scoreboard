# Developing for the Scoreboard

## Introduction

So you want to contribute!  Great!  Amazing!  Super fantastic!  Most of this work has, up to the point of this writing, at least, been a solo project, so there are potentially some wrinkles to get through.  If you encounter anything that doesn't work on another platform or isn't well documented, please at least open a ticket to let me know and I'll do my best to address it.  Bonus points if you're able to just fix the problem yourself and submit a pull request :)

## Windows

I do all of my development on Windows, so that's probably the lowest friction
platform to contribute from.  That said, many architecture decisions were made
to be specifically platform agnostic, so there should be no insurmountable
impediment to developing in any other platform.

### Software you'll definitely need

The following items are pretty much required to get anywhere as a developer on
this project.

* A compiler - I feel like I shouldn't have to say this, but here I am.  Saying
  it.
* [Cmake](https://cmake.org/) - Cmake is our build platform and you'll need it
  installed on your system to build anything.

### Software I recommend

You don't have to have an environment that looks like mine, but I like it, so
I'll enumerate a couple of other things that have worked well for me, just in
case anyone wants to know.

* [Visual Studio Community Edition](https://visualstudio.microsoft.com/vs/) -
  It's the IDE I use.  I've found it's integration with Cmake to be pretty
  good, and it's got a great debugger.
* [Vcpkg](https://github.com/microsoft/vcpkg) - A really solid package manager
  for visual studio.  NB: If you use Visual Studio Code, I was unable to get
  these to cooperate with each other for some reason.
* [The github VS extension](https://visualstudio.github.com/index.html) -- It
  makes interacting with the project on Github a lot more pleasant.

### Packages you'll need to install

If you're using vcpkg, you'll want to get these four packages from vcpkg.
Otherwise, you'll need to have these development packages somewhere that you
can point Cmake at them.  Note that for Windows development, you'll want to
get the 64 bit static versions of all of these libraries.

* [wxWidgets](https://www.wxwidgets.org/) - This is the UI framework the
  scoreboard uses.
* [Protocol buffers](https://developers.google.com/protocol-buffers) - The
  scoreboard uses protobuf for configuration storage and serialization.
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp) - Our autoupdater
  has to parse a json response to know what the latest released version of the
  application is.
* [Curl](https://curl.haxx.se/libcurl) - A pretty standard library for
  communicating with HTTP services.
* [GoogleTest](https://github.com/google/googletest) - Google's C++ unit
  testing framework.

If you're using vcpkg, the following command should get all four in one go:
```SHELL
vcpkg install curl:x64-windows-static gtest:x64-windows-static jsoncpp:x64-windows-static protobuf:x64-windows-static wxwidgets:x64-windows-static
```

You may also need to build wxWidgets from scratch and download the WebView runtime from Microsoft:
https://learn.microsoft.com/en-us/microsoft-edge/webview2/concepts/distribution#understand-the-webview2-runtime-and-installer-preview
When I did that, I needed to copy over
wxWidgets\out\build\x64-Debug\libs\webview\packages\Microsoft.Web.WebView2.1.0.864.35\build\native\x64\WebView2LoaderStatic.lib
to the install lib directory for that library to be found.

## Initial Setup

If you're using Visual Studio, you'll want to clone the project into a new
solution via the github plugin.  Then go to project->cmake settings.  I've
included a file named "DefaultCMakeSettings.json" that you can use to bootstrap
your CMakeSettings.json file for Visual Studio.  It'll require you to fill in
a few paths which will be specific to your system, the location of the vcpkg
toolchain file and the location of where vcpkg installed protobuf.  Each is
listed in both profiles (debug & release), so you'll change four variables.

## Linux

I've also developed on Ubuntu.  Here's what I did to get things compiling on Ubuntu.

### Packages you'll need to install

sudo apt install cmake cmake-curses-gui ninja-build g++ libprotobuf-dev protobuf-compiler libjsoncpp-dev libwxgtk3.0-dev

libcurl -- For libcurl I had to clone the repo from github for curl and run cmake && make all && sudo make install to get it to work correctly.  I also had to install libssl-dev via apt in order to get it to build properly.

wxWidgets -- for libwx, I also had to build this myself.  I cloned the github repo, configured cmake for Debug/gtk3.  Then I ran cmake && make all && sudo make install. 
I also had to install libx11-dev, libgtk-3-dev, libcairo2-dev, libjpeg-dev, libpng-dev, libexpat1-dev, libtiff-dev, libsecret-1-dev, libsoup2.4-dev, libmspack-dev, libsdl-dev, libsdl2-dev, libnotify-dev, and  libwebkit2gtk-4.0-dev via apt.

googletest -- In order to get GMock working correctly, I had to build this from source, because the built-in cmake files for gtest don't find gmock correctly.  This is a straight-forward clone/cmake/make all/make install with no real configuration needed.

