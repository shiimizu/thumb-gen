# Thumbnail generation

4chan uses PHP to generate their thumbnails (and run their site).  
PHP uses their own `libgd` (image processing library) which also uses their own modified version of `libjpeg-8d`.  

## Media Characteristics
Before 2017, thumbnails used the same jpeg quality: `60`.  
Since the start of 2017 and beyond, OP thumbnails use `50` and reply thumbnails use `40` as their jpeg image quality scale.  

**OP Thumbnails**
* Max width: `250`
* Max height: `250`
* JPG quality: `50`

**Reply Thumbnails**
* Max width: `125`
* Max height: `125`
* JPG quality: `40`

## Application

To generate checksum-compliant thumbnails you can either use PHP v5.4.15 or later — or manually build `libjpeg-8d` and `libgd` from source.

### PHP
```bash
$ php tg.php
$ b3sum pass-thumb-orig.jpg thumb/passs.jpg
596f127b22fdd937283e9a3e4defdba6819c995a721eb737d27227e109baf163  pass-thumb-orig.jpg
596f127b22fdd937283e9a3e4defdba6819c995a721eb737d27227e109baf163  thumb/passs.jpg

$ php tg.php
$ b3sum miss-thumb-orig.jpg thumb/misss.jpg
7715bcfa13d8bf5597daf7c51caf1dd4ebececce345829bc9700f1f321b4db93  miss-thumb-orig.jpg
7715bcfa13d8bf5597daf7c51caf1dd4ebececce345829bc9700f1f321b4db93  thumb/misss.jpg
```

### C
The C version is a simple implementation. It's usually hit or miss because PHP's GD is different from standard GD which results in slightly different colors + the C version currently does not account for image dimensions & jpeg quality checks.  
```bash
$ ./tg pass.jpg pass-thumb.jpg
$ b3sum pass-thumb-orig.jpg pass-thumb.jpg
596f127b22fdd937283e9a3e4defdba6819c995a721eb737d27227e109baf163  pass-thumb-orig.jpg
596f127b22fdd937283e9a3e4defdba6819c995a721eb737d27227e109baf163  pass-thumb.jpg

$ ./tg miss.jpg miss-thumb.jpg
$ b3sum miss-thumb-orig.jpg miss-thumb.jpg
7715bcfa13d8bf5597daf7c51caf1dd4ebececce345829bc9700f1f321b4db93  miss-thumb-orig.jpg
d3e6aa1c96a81867618134671f53a3820618151564c892e5eeeefaba03f6ddc2  miss-thumb.jpg
```

### Checking image information

__*Imagemagick:*__  
<sub>(Note: Installing Imagemagick also installs `libjpeg` and could mess with your `libjpeg-8d` installation. In that scenario, use the `--prefix=` flag when building `libjpeg-8d`)</sub>
```
identify -verbose -features 1 -moments -unique 1583219690557s.jpg > thumb.txt
```

### Building from source

<sup>(Iɴsᴛʀᴜᴄᴛɪᴏɴs ᴄᴜʀʀᴇɴᴛʟʏ ᴏɴʟʏ ғᴏʀ **Lɪɴᴜx**)</sup>

Currently, the only supported filetypes are JPEG images.  
GIF, PNG, etc are producing different checksums.  
The reasons the checksums still don't line up is because PHP's gd is different from standard gd.

Sources:
* PHP's [`libjpeg-8d`](https://github.com/winlibs/libjpeg/releases/tag/libjpeg-8d)<sup>[[1](https://wiki.php.net/internals/windows/libs/libjpeg)]</sup>
* [`libgd-2.0.35`](http://repository.timesys.com/buildsources/l/libgd/libgd-2.0.35/) or earlier
* [`libpng-1.2.50`](https://github.com/winlibs/libpng/releases/tag/libpng-1.2.50) <sub>(optional for now)</sub>

#### Build libjpeg

```bash
$ cd libjpeg-8d
$ mkdir build
$ ./configure --prefix=$(pwd)/build
$ make
$ sudo make install # Install to prefix
$ export JPEG_INCLUDE_DIR=$(pwd)/build
```

#### Build libpng

```bash
$ cd libpng-1.2.50
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_INSTALL_PREFIX=$(pwd)/build
$ make
$ sudo make install # Install to prefix
$ export PNG_INCLUDE_DIR=$(pwd)/build
# Then you have to remove the 12 in libpng12 in $PNG_INCLUDE_DIR/lib by copying files and resymlinking them
```

#### Build libgd

```bash
$ cd gd-2.0.35
$ mkdir build
$ autoreconf -fi
$ ./configure --with-jpeg=$JPEG_INCLUDE_DIR --with-png=$PNG_INCLUDE_DIR --x-includes=$PNG_INCLUDE_DIR/include --x-libraries=$PNG_INCLUDE_DIR/lib --with-xpm=no --with-x=no --with-freetype=no --with-fontconfig=no --prefix=$(pwd)/build
$ make
$ sudo make install # Install to prefix
```

#### Compilation
```bash
$ gcc -o tg tg.c -lgd -lpng -lz -ljpeg -lm -static

# Or if you installed the libs locally
$ gcc -o tg tg.c -I/path/to/gd-2.0.35/build/include -L/path/to/gd-2.0.35/build/lib -lgd -I/path/to/libjpeg-8d/build/include -L/path/to/libjpeg-8d/build/lib -ljpeg -I/path/to/libpng-1.2.50/build/include -L/path/to/libpng-1.2.50/build/build/ -lpng -lz -lm -static
```

--- 

## How did you do it?
I looked at the [Futaba](https://www.2chan.net)'s [source code](https://github.com/futoase/futaba-ng) which 4chan is based from + the 4chan source code leak and used the same code they used to generate thumbnails. After finding out which PHP version (5.4.15) was used, I ran `php --re gd` to get the libgd version (2.0.35) and `php --ri gd` to get the libjpeg (8d) & libpng (1.2.50) versions.

### Resources
* [Building PHP for Windows](https://wiki.php.net/internals/windows/stepbystepbuild)
* [php-sdk archives](https://windows.php.net/downloads/php-sdk/deps/archives)
* [PHP archives](https://windows.php.net/downloads/releases/archives/)