

# Building

There are two steps, described for each platform below

1. Build wxWidgets
2. Build FTK


## Building on Windows

Note this has only been tested with VC 2015. It probably works with other versions.

1. Start a command line with Visual Studio 2015 X64 command line tools.
2. cd to the 'ftk' folder (the one you checked out)
3. `python tools/build_wxwidgets.py`

When complete you'll have static libraries the following folder:
```
wxWidgets\lib\vc_x64_lib\wxmsw30u(d)*.lib
```

4. Run Visual Studio 2015
5. Open the solution `ftk/build/win/ftk_wx.sln`
6. F7 to build

## Building on OSX

1. cd to the 'ftk' folder (the one you checked out)
2. `python tools/build_wxwidgets.py`

You'll then have the following libraries:

```
wxWidgets/build-debug/lib/*.a
wxWidgets/build-release/lib/*.a
```

3. Run XCode
4. Open `ftk/build/osx/Flutter ToolKit.xcodeproj'
5. Cmd + B to build.

## Building on Linux

1. cd to the 'ftk' folder (the one you checked out)
2. `python tools/build_wxwidgets.py`
3. cd to ftk/build/linux
4. cmake .
5. make
 


