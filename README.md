# Thumbnail generation

Note:

Currently, the only supported filetypes are JPEG images.<br>
GIF, PNG, and WEBM are producing different hashsums.

## Tested:
* `php-5.3.20-Win32-VC9-x86` `libgd 2.0.34`

* `php-5.3.22-Win32-VC9-x86` `libgd 2.0.34`

* `php-5.3.29-nts-Win32-VC9-x86` `libgd 2.1.0`

* `php-5.4.11-nts-Win32-VC9-x86` `libgd 2.0.34`

* `php-5.4.36-nts-Win32-VC9-x86` `libgd 2.1.0`

* `php-5.4.45-nts-Win32-VC9-x86` `libgd 2.1.0`

Notes: Seems to only work with VC9 ✔️

__*Not working:*__
* any php version > php-5.4.45 (because it uses a higher `libgd`)
* any `libgd` version > `2.0.35` (although the signature and all the color parameters are the same, slight additions to the metadata seems to change the hashsum)
* any `libjpeg` version > `8d` (different algorithms for newer versions)
* `libjpeg-turbo` (different algorithm)
* python gdmodule
* python PIL
* python pillow

**New findings:**
When [building PHP for Windows](https://wiki.php.net/internals/windows/stepbystepbuild), in the [php-sdk archives](https://windows.php.net/downloads/php-sdk/deps/archives), you'll find that VC9 uses `libjpeg 8d`

# ✔️ *Newer findings:*

## Requirements:

* `libgd-2.0.34` or `libgd-2.0.35`
* A [patched `libjpeg-8d`](https://github.com/winlibs/libjpeg/releases/tag/libjpeg-8d)<sup>[[1](https://wiki.php.net/internals/windows/libs/libjpeg)]</sup>

## What are these?
`libjpeg` is a jpeg library.

`libgd` is an image processing library like imagemagick written in `C` and uses many libraries to process different image formats.

---

# Usage
### C
```
gcc -o thumbgen thumbgen.c -lgd -lpng -lz -ljpeg -lfreetype -lm -static
./thumbgen orig.jpg thumb.jpg
```

### PHP
```
php thumbgen.php
```

### Checking

__*Hashsums:*__
```
python hash.py orig_thumb.jpg
python hash.py thumb.jpg
```

__*Imagemagick:*__
<sub>(Note: Installing Imagemagick also installs `libjpeg` and could mess with your `libjpeg-8d` installation. In that scenario, use the `--prefix=` flag when building `libjpeg-8d`)</sub>
```
identify -verbose -features 1 -moments -unique thumb.jpg>thumb.txt
```
---
# Output
```bash
$ python hash.py orig_thumb.jpg
SHA256: gmYErtMAyZYZobhXZf5+npVf8ZFJJWr959CPHIift2A=
MD5: 4QIMUT+mn01qiR4cRLQANQ==

$ python hash.py thumb.jpg
SHA256: gmYErtMAyZYZobhXZf5+npVf8ZFJJWr959CPHIift2A=
MD5: 4QIMUT+mn01qiR4cRLQANQ==
```
---
# Installation

(Iɴsᴛʀᴜᴄᴛɪᴏɴs ᴄᴜʀʀᴇɴᴛʟʏ ᴏɴʟʏ ғᴏʀ **Lɪɴᴜx**)

### Build `libjpeg-8d`


__*Standard building:*__
```bash
cd libjpeg-libjpeg-8d
./configure
make
sudo make install # Install libjpeg to system
```

__*To specify the build output location:*__
```bash
cd libjpeg-libjpeg-8d
mkdir jpeg-build
./configure --prefix=$(pwd)/jpeg-build
make -j4
sudo make install # Output libjpeg to jpeg-build dir
```

### Build `libgd` `2.0.34` or `2.0.35`

__*Standard building:*__
```bash
cd libgd-2.0.35
./configure --with-png=no --with-xpm=no # auto look for libjpeg
make
sudo make install # Install libgd to system
```

__*To specify the build output location:*__
```bash
cd libgd-2.0.35
mkdir gd-build
./configure --prefix=$(pwd)/gd-build --with-png=no --with-xpm=no 
make -j4
sudo make install # Output libgd to gd-build dir
```

__*To use & specify the jpeg build from before:*__
```bash
./configure --with-png=no --with-xpm=no --with-jpeg=$(pwd)/../libjpeg-libjpeg-8d/jpeg-build
make -j4
sudo make install # Install libgd to system
```

__*To uninstall any of the libs:*__
`make uninstall`

---

# Dev scratchpad
Just for me. My notes in trying to figure all this out. Also me trying to compile the libs for windows.. >_>
```
It seems like thumbnail generation depends on the platform and buildtool (gcc/msvc)

http://www.multigesture.net/articles/how-to-upgrade-your-mingw-with-commonly-used-libraries/

https://www.daniweb.com/programming/software-development/threads/438160/png-include-directory-cmake
Buidling on MSVC
Open Developer command prompt
mkdir build-dir && cd build-dir
cmake ..
cmake --build . -j4
# Output is in Debug/static.lib
```

```
convert -strip -thumbnail '125x83' -units PixelsPerInch image -density 96 

Find libjpeg version in pillow
https://stackoverflow.com/a/24397115

ldd /home/shiimizu/.local/lib/python3.7/site-packages/PIL/_imaging.cpython-37m-x86_64-linux-gnu.so

wrting gdmodule
https://stackoverflow.com/a/16130031
```
```
cmake .. -DZLIB_LIBRARY=deps\zlib.lib -DPNG_LIBRARY=deps\libpng.lib -DJPEG_LIBRARY=deps\jpeg.dll -DFREETYPE_LIBRARY=deps\freetype.lib -DPTHREAD_DIR=deps\pthreads-w32-2-9-1-release\Pre-built.2
```
```
PHP libjpeg (What libjpeg version PHP uses)
https://wiki.php.net/internals/windows/libs/libjpeg
```
