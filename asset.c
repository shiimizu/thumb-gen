#include <koio.h>

static struct {
	const char *path;
	size_t len;
	char *data;
} files[] = {
	{
		.path = "//tg.php",
		.len = 3218,
		.data = ""
"<?php define(\"THUMB_DIR\",\"thumb/\");define(\"MAX_W\",\"250\");define(\"MAX_"
"H\",\"250\");define(\"MAXR_W\",\"125\");define(\"MAXR_H\",\"125\");define(\"EN"
"ABLE_PDF\",0);if(!($argc==3||$argc==4||$argc==5)){usage($argv[0]);return 1;}$f"
"ilename=$argv[1];$out=$argv[2];$reply=true;$sfw=true;if($argc>3){$reply=$argv["
"3];if($argc>4){$sfw=$argv[4];}}if(file_exists($filename)&&!is_dir($filename)){"
"thumb($filename,$out,$reply,$sfw);}else{printf(\"Unknown file: `$s`\\n\",$file"
"name);return 1;}function usage($self){printf(\"Generate thumbnails from 4chan\\"
"n\");printf(\"%s  <input-file>  <output-file>  [reply?1:0]  [sfw-board?1:0]\\n"
"\",$self);return;}function thumb($path,$outpath,$reply,$sfw){if(!function_exis"
"ts(\"ImageCreate\")||!function_exists(\"ImageCreateFromJPEG\"))return;$path_pa"
"rts=pathinfo($path);$fname=$path;if($reply){$width=MAXR_W;$height=MAXR_H;$jpg_"
"quality=40;}else{$width=MAX_W;$height=MAX_H;$jpg_quality=50;}if(ENABLE_PDF==1&"
"&$ext==\".pdf\"){$pdfjpeg=$outpath.\".pdf.tmp\";@exec(\"/usr/local/bin/gs -q -"
"dSAFER -dNOPAUSE -dBATCH -sDEVICE=jpeg -sOutputFile=$pdfjpeg $fname\");if(!fil"
"e_exists($pdfjpeg))unlink($fname);$fname=$pdfjpeg;}$size=GetImageSize($fname);"
"$memory_limit_increased=false;if($size[0]*$size[1]>3000000){$memory_limit_incr"
"eased=true;ini_set(\"memory_limit\",memory_get_usage()+$size[0]*$size[1]*10);}"
"switch($size[2]){case 1:if(function_exists(\"ImageCreateFromGIF\")){$im_in=Ima"
"geCreateFromGIF($fname);if($im_in){break;}}if(!is_executable(realpath(\"/www/g"
"lobal/gif2png\"))||!function_exists(\"ImageCreateFromPNG\"))return;@exec(realp"
"ath(\"/www/global/gif2png\").\" $fname\",$a);$im_in=ImageCreateFromPNG($fname)"
";unlink($fname);if(!$im_in)return;break;case 2:$im_in=ImageCreateFromJPEG($fna"
"me);if(!$im_in){return;}break;case 3:if(!function_exists(\"ImageCreateFromPNG\""
"))return;$im_in=ImageCreateFromPNG($fname);if(!$im_in){return;}break;default:r"
"eturn;}if($size[0]>$width||$size[1]>$height||$size[2]==1){$key_w=$width/$size["
"0];$key_h=$height/$size[1];($key_w<$key_h)?$keys=$key_w:$keys=$key_h;$out_w=fl"
"oor($size[0]*$keys);$out_h=floor($size[1]*$keys);}else{$out_w=$size[0];$out_h="
"$size[1];}if(function_exists(\"ImageCreateTrueColor\")&&get_gd_ver()==\"2\"){$"
"im_out=ImageCreateTrueColor($out_w,$out_h);}else{$im_out=ImageCreate($out_w,$o"
"ut_h);}switch($size[2]){case 1:case 3:if($reply){$color=$sfw?imagecolorallocat"
"e($im_out,214,218,240):imagecolorallocate($im_out,240,224,214);}else{$color=$s"
"fw?imagecolorallocatealpha($im_out,238,242,255,0):imagecolorallocate($im_out,2"
"55,255,238);}imagefill($im_out,0,0,$color);break;default:case 2:break;}ImageCo"
"pyResampled($im_out,$im_in,0,0,0,0,$out_w,$out_h,$size[0],$size[1]);ImageJPEG("
"$im_out,$outpath,$jpg_quality);chmod($outpath,0666);ImageDestroy($im_in);Image"
"Destroy($im_out);if(isset($pdfjpeg)){unlink($pdfjpeg);}if($memory_limit_increa"
"sed)ini_restore(\"memory_limit\");return $outpath;}function get_gd_ver(){if(fu"
"nction_exists(\"gd_info\")){$gdver=gd_info();$phpinfo=$gdver[\"GD Version\"];}"
"else{ob_start();phpinfo(8);$phpinfo=ob_get_contents();ob_end_clean();$phpinfo="
"strip_tags($phpinfo);$phpinfo=stristr($phpinfo,\"gd version\");$phpinfo=strist"
"r($phpinfo,\"version\");}$end=strpos($phpinfo,\".\");$phpinfo=substr($phpinfo,"
"0,$end);$length=strlen($phpinfo)-1;$phpinfo=substr($phpinfo,$length);return $p"
"hpinfo;} ?>",
	},
};

void koio_load_assets(void) {
	ko_add_file(files[0].path, files[0].data, files[0].len);
}

void koio_unload_assets(void) {
	ko_del_file(files[0].path);
}
