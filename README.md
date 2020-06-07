# Thumbnail creation

4chan uses PHP to generate their thumbnails (and run their site).  
PHP uses their own `libgd` (image processing library) which also uses their own `libjpeg-8d`.  

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

To generate checksum-compliant thumbnails you have the following options:  
* Use PHP v5.4.15 or later to run the PHP script
* Download the [latest](https://github.com/shiimizu/thumb-gen/releases/latest) the pre-built binaries  
* Manually build `libjpeg-8d` and `libgd` from source

### PHP
```console
$ php tg.php
$ Generate thumbnails from 4chan
tg.php  <input-file>  <output-file>  [reply?1:0]  [sfw-board?1:0]

$ php tg.php img/op.jpg op-new.jpg 0
$ b3sum img/op-thumb-orig.jpg op-new.jpg
e039f64581ee448e8230146272621b4cc9891d1901f42359b66a76c0ee17916b  img/op-thumb-orig.jpg
e039f64581ee448e8230146272621b4cc9891d1901f42359b66a76c0ee17916b  thumb/ops.jpg

$ php tg.php img/reply.jpg reply-new.jpg
$ b3sum img/reply-thumb-orig.jpg reply-new.jpg
7715bcfa13d8bf5597daf7c51caf1dd4ebececce345829bc9700f1f321b4db93  img/reply-thumb-orig.jpg
7715bcfa13d8bf5597daf7c51caf1dd4ebececce345829bc9700f1f321b4db93  reply-new.jpg

$ php tg.php img/alpha-new.jpg 0
$ b3sum img/alpha-thumb-orig.jpg alpha-new.jpg
59ddd640d38aa3f9e40ff81e35755877d2fa4b2f9371ac86572452e85f204701  img/alpha-thumb-orig.jpg
59ddd640d38aa3f9e40ff81e35755877d2fa4b2f9371ac86572452e85f204701  alpha-new.jpg

$ php tg.php img/dance.gif dance-new.jpg 1 0
$ b3sum img/dance-thumb-orig.jpg dance-new.jpg
4270149933bcb5d5cac3bb5934c4422249b38e73c3347ee5f8377d8235cb0c3f img/dance-thumb-orig.jpg
4270149933bcb5d5cac3bb5934c4422249b38e73c3347ee5f8377d8235cb0c3f dance-new.jpg
```

### C  
On rare occasions, animated gifs currently produce different results.  
```console
$ ./tg
$ Generate thumbnails from 4chan
./tg  <input-file>  <output-file>  [reply?1:0]  [sfw-board?1:0]

$ ./tg img/op.jpg op-new.jpg 0
$ b3sum img/op-thumb-orig.jpg op-new.jpg
e039f64581ee448e8230146272621b4cc9891d1901f42359b66a76c0ee17916b  img/op-thumb-orig.jpg
e039f64581ee448e8230146272621b4cc9891d1901f42359b66a76c0ee17916b  op-new.jpg

$ ./tg img/reply.jpg reply-new.jpg
$ b3sum img/reply-thumb-orig.jpg reply-new.jpg
7715bcfa13d8bf5597daf7c51caf1dd4ebececce345829bc9700f1f321b4db93  img/reply-thumb-orig.jpg
7715bcfa13d8bf5597daf7c51caf1dd4ebececce345829bc9700f1f321b4db93  reply-new.jpg

$ ./tg img/alpha.png alpha-new.jpg 0
$ b3sum img/alpha-thumb-orig.jpg alpha-new.jpg
59ddd640d38aa3f9e40ff81e35755877d2fa4b2f9371ac86572452e85f204701  img/alpha-thumb-orig.jpg
59ddd640d38aa3f9e40ff81e35755877d2fa4b2f9371ac86572452e85f204701  alpha-new.jpg

$ ./tg img/dance.gif dance-new.jpg 1 0
$ b3sum img/dance-thumb-orig.jpg dance-new.jpg
4270149933bcb5d5cac3bb5934c4422249b38e73c3347ee5f8377d8235cb0c3f  img/dance-thumb-orig.jpg
ac4988a8e7b724dbe2c31c54914d25e1cab641a9d899c26900bf8d4970f791b4  dance-new.jpg
```

### Checking image information

__*Imagemagick:*__  
<sub>(Note: Installing Imagemagick also installs `libjpeg` and could mess with your `libjpeg-8d` installation. In that scenario, use the `--prefix=` flag when building `libjpeg-8d`)</sub>
```
identify -verbose -features 1 -moments -unique 1583219690557s.jpg > thumb.txt
```

### Building from source

<sup>(Iɴsᴛʀᴜᴄᴛɪᴏɴs ᴄᴜʀʀᴇɴᴛʟʏ ᴏɴʟʏ ғᴏʀ **Lɪɴᴜx**)</sup>

Sources:
* [`libjpeg-8d`](https://github.com/winlibs/libjpeg/releases/tag/libjpeg-8d)<sup>[[1](https://wiki.php.net/internals/windows/libs/libjpeg)]</sup>
* [`libgd-2.0.35`](http://repository.timesys.com/buildsources/l/libgd/libgd-2.0.35/) or earlier
* [`libpng-1.2.50`](https://github.com/winlibs/libpng/releases/tag/libpng-1.2.50)

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
$ cp scripts/makefile.linux Makefile # Then edit the Makefile and change the prefix to $(pwd)/build
$ make
$ sudo make install # Install to prefix
$ export PNG_INCLUDE_DIR=$(pwd)/build
```

#### Build libgd

```bash
$ cd libgd-2.0.35 # Then apply the patches from the repo 
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
$ gcc -o tg tg.c -I/path/to/libgd-2.0.35/build/include -L/path/to/libgd-2.0.35/build/lib -lgd -I/path/to/libjpeg-8d/build/include -L/path/to/libjpeg-8d/build/lib -ljpeg -I/path/to/libpng-1.2.50/build/include -L/path/to/libpng-1.2.50/build/build/ -lpng -lz -lm -static
```

--- 

## How did you do it?
I looked at the [Futaba](https://www.2chan.net)'s [source code](https://github.com/futoase/futaba-ng) which 4chan is based from + the 4chan source code leak and used the same code they used to generate thumbnails. After finding out which PHP version (5.4.15) was used, I ran `php --re gd | grep GD` to get the libgd version (2.0.35) and `php --ri gd` to get the libjpeg (8d) & libpng (1.2.50) versions.

### Resources
* [Building PHP for Windows](https://wiki.php.net/internals/windows/stepbystepbuild)
* [php-sdk archives](https://windows.php.net/downloads/php-sdk/deps/archives)
* [PHP archives](https://windows.php.net/downloads/releases/archives/)
