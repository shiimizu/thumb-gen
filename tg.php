<?php
define("THUMB_DIR", 'thumb/'); //サムネイル保存ディレクトリ
define("MAX_W", '250'); //投稿サイズ幅（これ以上はwidthを縮小
define("MAX_H", '250');
define("MAXR_W", '125');
define("MAXR_H", '125');
define("ENABLE_PDF", 0);

thumb("./", "dance", ".gif", false);

// resto -> For replies: this is the ID of the thread being replied to. For OP: this value is zero
function thumb($path, $tim, $ext, $resto)
{
    if (!function_exists("ImageCreate") || !function_exists("ImageCreateFromJPEG"))
        return;
    $fname     = $path . $tim . $ext;
    $thumb_dir = THUMB_DIR; //thumbnail directory
    $outpath   = $thumb_dir . $tim . 's.jpg';
    if( is_dir($thumb_dir) === false ) {
        mkdir($thumb_dir);
    }
    if (!$resto) {
        $width  = MAX_W; //output width
        $height = MAX_H; //output height
        $jpg_quality = 50;
    } else {
        $width  = MAXR_W; //output width (imgreply)
        $height = MAXR_H; //output height (imgreply)
        $jpg_quality = 40;
    }
    // width, height, and type are aquired
    if (ENABLE_PDF == 1 && $ext == '.pdf') {
        // create jpeg for the thumbnailer
        $pdfjpeg = $path . $tim . '.pdf.tmp';
        @exec("/usr/local/bin/gs -q -dSAFER -dNOPAUSE -dBATCH -sDEVICE=jpeg -sOutputFile=$pdfjpeg $fname");
        if (!file_exists($pdfjpeg))
            unlink($fname);
        $fname = $pdfjpeg;
    }
    $size                   = GetImageSize($fname);
    $memory_limit_increased = false;
    if ($size[0] * $size[1] > 3000000) {
        $memory_limit_increased = true;
        ini_set('memory_limit', memory_get_usage() + $size[0] * $size[1] * 10); // for huge images
    }
    switch ($size[2]) {
        case 1:
            if (function_exists("ImageCreateFromGIF")) {
                $im_in = ImageCreateFromGIF($fname);
                if ($im_in) {
                    break;
                }
            }
            if (!is_executable(realpath("/www/global/gif2png")) || !function_exists("ImageCreateFromPNG"))
                return;
            @exec(realpath("/www/global/gif2png") . " $fname", $a);
            if (!file_exists($path . $tim . '.png'))
                return;
            $im_in = ImageCreateFromPNG($path . $tim . '.png');
            unlink($path . $tim . '.png');
            if (!$im_in)
                return;
            break;
        case 2:
            $im_in = ImageCreateFromJPEG($fname);
            if (!$im_in) {
                return;
            }
            break;
        case 3:
            if (!function_exists("ImageCreateFromPNG"))
                return;
            $im_in = ImageCreateFromPNG($fname);
            if (!$im_in) {
                return;
            }
            break;
        default:
            return;
    }
    // Resizing
    if ($size[0] > $width || $size[1] > $height || $size[2] == 1) {
        $key_w = $width / $size[0];
        $key_h = $height / $size[1];
        ($key_w < $key_h) ? $keys = $key_w : $keys = $key_h;
        $out_w = floor($size[0] * $keys);
        $out_h = floor($size[1] * $keys);
        /*if ($size[2]==1) {
        $out_w = $size[0];
        $out_h = $size[1];
        } //what was this for again? */
    } else {
        $out_w = $size[0];
        $out_h = $size[1];
    }
    // the thumbnail is created
    if (function_exists("ImageCreateTrueColor") && get_gd_ver() == "2") {
        $im_out = ImageCreateTrueColor($out_w, $out_h);
    } else {
        $im_out = ImageCreate($out_w, $out_h);
    }

    // Transparency
    switch ($size[2]) {
        case 1:
        case 3:
            // echo "gif or png\n";
            // $color = imagecolorallocate($im_out,  255, 255, 238);            // orange board op      // FFFFEE
            $color = imagecolorallocate($im_out,  240, 224, 214);               // orange board reply   // F0E0D6
            // $color = imagecolorallocatealpha($im_out,  238, 242, 255, 0);    // blue board OP        // EEF2FF
            // $color = imagecolorallocate($im_out,  214, 218, 240);            // blue board reply     // D6DAF0
            imagefill($im_out, 0, 0, $color);
            break;
        default:
        case 2:
            // echo "jpg\n";
            break;
    }


    // copy resized original
    ImageCopyResampled($im_out, $im_in, 0, 0, 0, 0, $out_w, $out_h, $size[0], $size[1]);

    // thumbnail saved
    ImageJPEG($im_out, $outpath, $jpg_quality);
    chmod($thumb_dir.$tim.'s.jpg',0666);

    // created image is destroyed
    ImageDestroy($im_in);
    ImageDestroy($im_out);
    if (isset($pdfjpeg)) {
        unlink($pdfjpeg);
    } // if PDF was thumbnailed delete the orig jpeg
    
    if ($memory_limit_increased)
        ini_restore('memory_limit');
    
    return $outpath;
}

//check version of gd
function get_gd_ver()
{
    if (function_exists("gd_info")) {
        $gdver   = gd_info();
        $phpinfo = $gdver["GD Version"];
    } else { //earlier than php4.3.0
        ob_start();
        phpinfo(8);
        $phpinfo = ob_get_contents();
        ob_end_clean();
        $phpinfo = strip_tags($phpinfo);
        $phpinfo = stristr($phpinfo, "gd version");
        $phpinfo = stristr($phpinfo, "version");
    }
    $end     = strpos($phpinfo, ".");
    $phpinfo = substr($phpinfo, 0, $end);
    $length  = strlen($phpinfo) - 1;
    $phpinfo = substr($phpinfo, $length);
    return $phpinfo;
}

?> 
