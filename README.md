
## gatling

<p align="middle">
  <a href="http://pablode.com/gatling/wwc_mustang.png"><img width=520 src="http://pablode.com/gatling/wwc_mustang_sm.png" /></a>
</p>
<p align="middle">
  1965 Ford Mustang Fastback from <a href="https://wirewheelsclub.com/models/1965-ford-mustang-fastback/">Wire Wheels Club</a>, rendered in Gatling.
</p>

### About

This is my toy path tracer I work on in my free time.

It is exposed as a Hydra render delegate and comes with a standalone that accepts [Universal Scene Description](https://graphics.pixar.com/usd/release/intro.html) (USD) files. It is cross-platform\*, GPU-accelerated, and implements the [MaterialX](https://www.materialx.org/index.html), [NVIDIA MDL](https://www.nvidia.com/en-us/design-visualization/technologies/material-definition-language/) and [UsdPreviewSurface](https://graphics.pixar.com/usd/release/spec_usdpreviewsurface.html) material standards.

Complex BSDFs like Autodesk's Standard Surface and the glTF shading model are supported using MaterialX's MDL code generation backend.  The MDL SDK then generates evaluation and importance sampling functions as GLSL code, which is compiled to SPIR-V and executed using Vulkan.

\* Hardware ray tracing is required. MacOS will be supported [in the future](https://github.com/KhronosGroup/MoltenVK/issues/427).

### Build

You need to

- download the <a href="https://developer.nvidia.com/nvidia-mdl-sdk-get-started">MDL SDK</a> (2022.0+) binaries
- download or build <a href="https://github.com/PixarAnimationStudios/USD/tree/v23.05">USD</a> (22.08+) with MaterialX support
- have NASM 2.13+ or YASM 1.2.0+ in your PATH

Do a recursive clone of the repository and set up a build folder:
```
git clone https://github.com/pablode/gatling --recursive
mkdir gatling/build && cd gatling/build
```

Pass following parameters in the CMake generation phase:
```
cmake .. -Wno-dev \
         -DUSD_ROOT=<USD_INSTALL_DIR> \
         -DMDL_ROOT=<MDL_INSTALL_DIR> \
         -DCMAKE_INSTALL_PREFIX=<USD_INSTALL_DIR>/plugin/usd
         -DCMAKE_BUILD_TYPE=Release
```

> Note: If you're using MSVC, be sure to select a 64-bit generator.

Build the relevant targets and install the Hydra delegate to the USD plugin folder:
```
cmake --build . -j8 --target hdGatling gatling --config Release
cmake --install . --component hdGatling
```

### Usage

Gatling can be used by every application which supports Hydra, either natively or through a plugin.

<p align="middle">
  <a href="http://pablode.com/gatling/usdview_coffeemaker.png"><img width=400 src="http://pablode.com/gatling/usdview_coffeemaker_sm.png" /></a>
</p>
<p align="middle">
  Claas Kuhnen's <a href="https://github.com/pablode/usd-assets/tree/main/coffeemaker">Coffee Maker</a>, rendered using Gatling inside Pixar's <a href="https://openusd.org/release/toolset.html#usdview">usdview</a> tool.
</p>

A headless standalone is provided that accepts a USD file (.usd, .usda, .usdc, .usdz) as input. Make sure that there is a polygonal light source in the scene.

```
./bin/gatling <scene.usd> render.png \
    --image-width 1200 \
    --image-height 1200 \
    --spp 1024 \
    --max-bounces 8
```

> Note: Disable the system's GPU watchdog or set an appropriate timeout value.

### Outlook

Next up: double-sided surfaces, analytic lights.

### License

Gatling is licensed under the GNU General Public License, as included in the [LICENSE](LICENSE) file.

```

    Copyright (C) 2019-2022 Pablo Delgado Krämer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.

```

It contains code from the Ray Tracing Gems I book, which is [MIT](docs/licenses/LICENSE.MIT.rtgems) licensed and copyrighted:

* Copyright 2019 NVIDIA Corporation

It contains code from the MDL SDK, which is [BSD](docs/licenses/LICENSE.BSD-3.mdl-sdk) licensed and copyrighted:

* Copyright (c) 2018-2021, NVIDIA CORPORATION. All rights reserved.
