<?php
var_dump(gd_info());
// imagecopyresampled from example 1
// File and new size
$filename = "00aa5b6f0cad3600bbfc1251e51462be.jpg";


// Get new dimensions
list($width, $height) = getimagesize($filename);
$new_width = 125;
$new_height = 83;

// Resample
$image_p = imagecreatetruecolor($new_width, $new_height);
$image = imagecreatefromjpeg($filename);
imagecopyresampled($image_p, $image, 0, 0, 0, 0, $new_width, $new_height, $width, $height);

// Output
imagejpeg($image_p, 'output.jpg', 40);

// Free up memory
imagedestroy($image_p);

?>