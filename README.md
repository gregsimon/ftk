

# Building

## Common (all platforms)

Pull down the 3.x branch of wxWidgets:
```
python tools/build_wxwidgets.py
```


## Building on Windows

Note this has only been tested with VC 2015. It probably works with other versions.

1. Open Visual Studio 2015.
2. Open `wxWidgets\\build\\msw\\wx_vc12.dsw`
3. Select "Release" and "x64"
4. Build (press F7)

## Building on OSX

There is nothing to do -- the python script you ran above took care of it. You'll have the following libraries:

```
wxWidgets/build-debug/lib/*.a
wxWidgets/build-release/lib/*.a
```



