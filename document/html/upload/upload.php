<?php
if (isset($_SERVER['PATH_INFO'])) {
	$pathInfo = $_SERVER['PATH_INFO'];
	echo "PATH_INFO: $pathInfo<br>";
} else {
	echo "PATH_INFO does not exist<br>";
}
#$targetDirectory = "/tmp/";
$targetDirectory = $_SERVER['PATH_INFO'] . '/';
#$targetDirectory = $_SERVER['PATH_INFO'];
$targetFile = $targetDirectory . uniqid() . '_' . basename($_FILES["fileToUpload"]["name"]);

if (move_uploaded_file($_FILES["fileToUpload"]["tmp_name"], $targetFile)) {
    echo "The file ". basename( $_FILES["fileToUpload"]["name"]). " has been uploaded.";
} else {
	echo "Sorry, there was an error uploading your file.";
}
?>
