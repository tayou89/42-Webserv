#!/usr/bin/perl

use CGI qw(:standard);
use CGI::Carp 'fatalsToBrowser';
use File::Basename;
use HTML::Entities;
# require "/Users/tayou/Desktop/tayou/42_webserv/document/cgi/upload/upload_util.pl";
require "/Users/jinhyeop/Desktop/project/webserv/document/cgi/upload/upload_util.pl";
# require "/Users/yejlee2/42cursus/webserv/document/cgi/upload/upload_util.pl";

print header, start_html("File Upload");

my $q = new CGI;
my $fileToUpload = &getFormData();
my $pathInfo = &getPathInfo();
my ($filename, $uploadFilehandle) = &getFileData($q, 'fileToUpload');
my $uploadedFile = &createUploadedFile($filename, $pathInfo);
&copyFile($uploadFilehandle, $uploadedFile);

print "The file $filename has been uploaded.";

print end_html;
