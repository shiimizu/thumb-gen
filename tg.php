<?php
define("THUMB_DIR", 'thumb/');	//サムネイル保存ディレクトリ
define("MAX_W", '250'); 		//投稿サイズ幅（これ以上はwidthを縮小
define("MAX_H", '250');
define("MAXR_W", '125');
define("MAXR_H", '125');
define("ENABLE_PDF", 0);

if (!($argc == 3 || $argc == 4 || $argc == 5)) {
    usage($argv[0]);
    return 1;
}

// Defaults
$filename = $argv[1];
$out = $argv[2];
$reply = true;
$sfw = true;

if ($argc > 3) {
    $reply = $argv[3];
    if ($argc > 4) {
        $sfw = $argv[4];
    }
}

if (file_exists($filename)) {
    thumb($filename, $out, $reply, $sfw);
} else {
    printf("`%s` does not exist.", $filename);
    return 1;
}

function usage($self) {
    printf("Generate thumbnails from 4chan\n");
    printf("%s  <input-file>  <output-file>  [reply?1:0]  [sfw-board?1:0]\n", $self);
    return;
}

// reply -> 0 if OP, true if it's a reply post.
function thumb($path, $outpath, $reply, $sfw)
{
    if (!function_exists("ImageCreate") || !function_exists("ImageCreateFromJPEG"))
        return;
	$path_parts = pathinfo($path);
    $fname     = $path;
    if ($reply) {
        $width  = MAXR_W; //output width (imgreply)
        $height = MAXR_H; //output height (imgreply)
        $jpg_quality = 40;
    } else {
        $width  = MAX_W; //output width
        $height = MAX_H; //output height
        $jpg_quality = 50;
    }
    // width, height, and type are aquired
    if (ENABLE_PDF == 1 && $ext == '.pdf') {
        // create jpeg for the thumbnailer
        $pdfjpeg = $outpath . '.pdf.tmp';
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
            // if (!file_exists($fname))
                // return;
            $im_in = ImageCreateFromPNG($fname);
            unlink($fname);
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

    // Transparency  to solid color
    switch ($size[2]) {
        case 1:
        case 3:
            // echo "gif or png\n";
	        if ($reply) { /* Reply */
	            // blue board reply     // D6DAF0
        		// or
				// orange board reply   // F0E0D6
       			$color = $sfw ? imagecolorallocate($im_out,  214, 218, 240) : imagecolorallocate($im_out,  240, 224, 214);
	        } else { /* OP */
            	// blue board OP        // EEF2FF
            	// or
            	// orange board op      // FFFFEE
        		$color = $sfw ? imagecolorallocatealpha($im_out,  238, 242, 255, 0) : imagecolorallocate($im_out,  255, 255, 238);
	        }
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
    chmod($outpath,0666);

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
