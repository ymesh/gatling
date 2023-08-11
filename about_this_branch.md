# ymesh_dev branch
This is my system environment:
```
Linux Fedora 34 (5.17.12-100.fc34.x86_64)
CMake v3.20.5
GCC v11.3.1 20220421 (Red Hat 11.3.1-2)
NASM v2.15.0
Python v3.9.13
Boost v1.75.0
OpenUSD v0.23.8
OpenEXR v2.4.0
MaterialX v1.38.7
MDL-SDK 367100.2992
```
### Changes:
- [CMakeLists.txt](CMakeLists.txt)<br>
USD 0.23.8 have compile error with c++ 20, so set it to 17<br>
```set(CMAKE_CXX_STANDARD 17)```<br>
- added [cmake/FindOpenEXR.cmake](cmake/FindOpenEXR.cmake)
- [src/imgio/CMakeLists.txt](src/imgio/CMakeLists.txt)<br>
as my USD build uses OpenEXR-v2.4.0, then set __OPENEXR_LOCATION__ environment variable in build script to locate it instead of building __Imath__ and __OpenEXR__ from externals<br>
```
find_package(OpenEXR REQUIRED)
```
```
target_include_directories(
  imgio
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OPENEXR_INCLUDE_DIRS}
  PRIVATE
    src
)
```
```
target_link_libraries(
  imgio
  PRIVATE
    spng
    turbojpeg-static
    # OpenEXR::OpenEXR
    ${OPENEXR_LIBRARIES}
    stb
)
```
- [extern/CMakeLists.txt](extern/CMakeLists.txt)<br>
disable __Imath__ and __OpenEXR__ build
```
#
# Imath
#
# add_subdirectory(Imath)

#
# OpenEXR
#
# set(OPENEXR_INSTALL OFF)
# set(OPENEXR_INSTALL_TOOLS OFF)
# set(OPENEXR_INSTALL_EXAMPLES OFF)
# add_subdirectory(OpenEXR)

```
- to prevent compiler complains about not defined __memcpy__ and __strcmp__ in the scope,  add  ```#include <cstring>```  to:<br>
[src/ggpu/include/denseDataStore.h](src/ggpu/include/denseDataStore.h)<br>
[src/ggpu/src/stager.cpp](src/ggpu/src/stager.cpp)<br>
[src/gi/src/texsys.cpp](src/gi/src/texsys.cpp)<br><br>
- [src/gi/src/gi.cpp:309](src/gi/src/gi.cpp)<br>
add ```std::``` to ```static_pointer_cast```
- [src/hdGatling/RenderPass.cpp:429](src/hdGatling/RenderPass.cpp)
due to strange compiler errors leading to standart cxx11 hash function, I've changed declaration of __TfHashMap__:
```
// XXX: TfHashMap<std::string, uint32_t> materialMap;
  TfHashMap<const char *, uint32_t> materialMap;
```
Hence, all ```materialMap``` indexes in code were changed from __std::string__ to __const char *__ (e.g. ```materialMap[materialIdStr.c_str()]```) 
