
# Build instructions

- You can build the project only on and for the darwin platform
- Building on linux, with darling is experimental
- Its easy to use directly in projects
- Compiling as a dylib can help minifying file size of your project
- Tip: use -O2 to optimize the code with clang
- Build instructions below aren't tested yet, soon will do that

### Dylib
```bash
clang mvusblib.c -dynamic -shared -O2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/SDKs/MacOSX.sdk/ -framework IOKit -framework CoreFoundation -o 
mvusblib.dylib
```

### Static library
```bash
clang mvusblib.c -r -O2 -o mvusblib.a -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/SDKs/MacOSX.sdk/
```

### On iOS
- Same as on mac. But change isysroot to your sdk path, mostlikely $THEOS/sdks/iPhoneOS*


## issues
- You need the IOKit and libkern headers when compiling for iOS. They can be copied from the macOS SDK.
- The project currently does not work on iOS because the usb service cannot be found. A solution is being researched.
- AppleSynopsisOTG is not opensource and has no symbols, I need help reversing the kext on iOS
