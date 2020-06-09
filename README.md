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
* Download the [latest](https://github.com/shiimizu/thumb-gen/releases/latest) pre-built binaries  
* Manually build from source

### PHP
```console
$ php tg.php
Generate thumbnails from 4chan
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
4270149933bcb5d5cac3bb5934c4422249b38e73c3347ee5f8377d8235cb0c3f  img/dance-thumb-orig.jpg
4270149933bcb5d5cac3bb5934c4422249b38e73c3347ee5f8377d8235cb0c3f  dance-new.jpg
```

### C binary executable 
```console
$ ./tg
Generate thumbnails from 4chan
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
4270149933bcb5d5cac3bb5934c4422249b38e73c3347ee5f8377d8235cb0c3f  dance-new.jpg
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
* `libzlib` (optional)
* [`libpng-1.2.50`](https://github.com/winlibs/libpng/releases/tag/libpng-1.2.50) (optional)
* [`libjpeg-8d`](https://github.com/winlibs/libjpeg/releases/tag/libjpeg-8d)<sup>[[1](https://wiki.php.net/internals/windows/libs/libjpeg)]</sup>
* [`php5`](https://museum.php.net/php5/)
* [`koio`](https://drewdevault.com/2018/05/29/Embedding-files-in-C.html)

#### Build libzlib

```bash
$ cd zlib-1.2.11
$ ./configure --prefix=$(pwd)/build --static --64
$ make
$ sudo make install # Install to prefix
$ export ZLIB_DIR=$(pwd)/build
```

#### Build libpng

```bash
$ cd libpng-1.2.50
$ cp scripts/makefile.linux Makefile # Then edit the Makefile and change the prefix to $(pwd)/build
$ make
$ sudo make install # Install to prefix
$ mkdir -p build/include build/lib
$ export PNG_DIR=$(pwd)/build
$ cp *.h $PNG_DIR/include
$ cp *.a *.so $PNG_DIR/lib
```

#### Build libjpeg

```bash
$ cd libjpeg-8d
$ ./configure --prefix=$(pwd)/build --enable-static
$ make
$ sudo make install # Install to prefix
$ export JPEG_DIR=$(pwd)/build
```

#### Build php5

```bash
$ cd php-5.4.15
$ ./buildconf --force

# If the configure below doesn't work try
# 	autoreconf -fi
# or
# 	aclocal; libtoolize --force; autoheader; autoconf
# and run configure again

$ CFLAGS="-I$ZLIB_DIR/include -I$PNG_DIR/include -I$JPEG_DIR/include" LDFLAGS="-L$ZLIB_DIR/lib -L$PNG_DIR/lib -L$JPEG_DIR/lib" ./configure --prefix=$(pwd)/dist --with-zlib=$ZLIB_DIR --with-zlib-dir=$ZLIB_DIR  --with-png-dir=$PNG_DIR --with-jpeg-dir=$JPEG_DIR --disable-all --disable-libxml --disable-dom --disable-simplexml --disable-xml --disable-xmlreader --disable-xmlwriter --without-pear --without-iconv --enable-debug --enable-cli --with-gd --enable-static --enable-embed=static

# For static builds: --enable-static --enable-embed=static

$ make -j 4
$ make install -j 4
$ export PHP_DIR=$(pwd)/build
```

#### Build koio

```bash
$ meson setup build --prefix=/ --buildtype=release --default-library=both
$ ninja -C build
$ DESTDIR=../dist/ ninja -C build install
$ export KOIO_DIR=$(pwd)/dist
# Then fix the library folders in dist/lib
```

#### Compilation

Finally. Let's compile our program.

[minify](https://php-minify.com/) the php script then run [koio](https://drewdevault.com/2018/05/29/Embedding-files-in-C.html) <sup>(yes you have to build this too)</sup> to generate a c source file that embeds the script

```bash
$ koio -o asset.c tg.php://tg.php
```

```bash
$ cc -o tg tg.c asset.c -lphp5 -lpng -ljpeg -lz -lkoio -static

# Or if you installed the libs locally
$ cc -o tg tg.c asset.c -I$PHP_DIR/include -I$PHP_DIR/include/php -I$PHP_DIR/include/php/include -I$PHP_DIR/include/php/main -I$PHP_DIR/include/php/Zend -I$PHP_DIR/include/php/TSRM -I$PHP_DIR/include/php/main/streams -I$ZLIB_DIR/include -I$PNG_DIR/include -I$JPEG_DIR/include -L$ZLIB_DIR/lib -L$PNG_DIR/lib -L$JPEG_DIR/lib -L$PHP_DIR/lib -I$KOIO_DIR/include -L$KOIO_DIR/lib -lphp5 -lpng -ljpeg -lz -lkoio -static
```

--- 
## How does it work?
PHP is built and statically linked against [`musl`](https://musl.cc). Using the `Zend` (PHP's core engine) API in our C code allows us to run an identical VM that made the original thumbnails.

> But that's basically just the PHP CLI then?!  

This is statically linked, and smaller, hence portable. Furthermore, it has one clear purpose, rather than being a general script runner/interpreter. You could say the PHP script was packaged into a single binary.  


## How did you do it?
I looked at the [Futaba](https://www.2chan.net)'s [source code](https://github.com/futoase/futaba-ng) which 4chan is based from + the 4chan source code leak and used the same code they used to generate thumbnails. After finding out which PHP version (5.4.15) was used, I ran `php --re gd | grep GD` to get the libgd version (2.0.35) and `php --ri gd` to get the libjpeg (8d) & libpng (1.2.50) versions.

### Resources
* [PHP5 archives](https://museum.php.net/php5/)
* [Building PHP for Windows](https://wiki.php.net/internals/windows/stepbystepbuild)
* [php-sdk archives](https://windows.php.net/downloads/php-sdk/deps/archives)
* [PHP Windows archives](https://windows.php.net/downloads/releases/archives/)
