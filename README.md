

# Building

## Common (all platforms)

Pull down the 3.x branch of wxWidgets:
```
python tools/build_wxwidgets.py
```


## Building on Windows

Note this has only been tested with VC 2015. It probably works with other versions.

1. Start a command line with Visual Studio 2015 X64 command line tools.
2. cd to `wxWidgets\build\msw\`
3. `nmake /f makefile.vc TARGET_CPU=X64 BUILD=release`
4. `nmake /f makefile.vc TARGET_CPU=X64`

When complete you'll have static libraries the following folder:
```
wxWidgets\lib\vc_x64_lib\*.lib
```

## Building on OSX

There is nothing to do -- the python script you ran above took care of it. You'll have the following libraries:

```
wxWidgets/build-debug/lib/*.a
wxWidgets/build-release/lib/*.a
```



