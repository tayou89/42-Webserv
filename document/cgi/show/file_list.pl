#!/usr/bin/perl

use strict;
use warnings;
use Cwd 'abs_path';
use File::Basename;
# require '/Users/tayou/Desktop/tayou/42_webserv/document/cgi/show/file_list_util.pl';
require '/Users/jinhyeop/Desktop/project/webserv/document/cgi/show/file_list_util.pl';
# require '/Users/yejlee2/42cursus/webserv/document/cgi/show/file_list_util.pl';

print "Content-Type: text/html\r\n\r\n";

# my $uploadDir = '/Users/tayou/Desktop/tayou/42_webserv/document/uploaded';
my $uploadDir = '/Users/jinhyeop/Desktop/project/webserv/document/uploaded';
# my $uploadDir = '/Users/yejlee2/42cursus/webserv/document/uploaded';
opendir (my $dh, $uploadDir) or die "Cannot open directory: $!";
my @files = grep { !/^\./ && -f "$uploadDir/$_" } readdir($dh);
closedir($dh);

my $htmlStringList = getHTMLStringList(@files);

# open (my $htmlFile, '<', '/Users/tayou/Desktop/tayou/42_webserv/document/html/file_list.html') or die "Cannot open HTML template: $!";
open (my $htmlFile, '<', '/Users/jinhyeop/Desktop/project/webserv/document/html/file_list.html') or die "Cannot open HTML template: $!";
# open (my $htmlFile, '<', '/Users/yejlee2/42cursus/webserv/document/html/file_list.html') or die "Cannot open HTML template: $!";
my $htmlContent = do { local $/; <$htmlFile> };
$htmlContent =~ s/{fileList}/$htmlStringList/g;

print $htmlContent;
