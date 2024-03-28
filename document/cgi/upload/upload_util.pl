use CGI;
use File::Basename;
use HTML::Entities;

sub getFormData {
    my $q = new CGI;
    my $fileToUpload = $q->param('fileToUpload');
    unless ($fileToUpload) {
        print "Error: couldn't find file to upload\n";
        exit(0);
    }
    return $fileToUpload;
}

sub getPathInfo {
    my $path_info = $ENV{'PATH_INFO'};
    unless ($path_info) {
        print "PATH_INFO does not exist<br>\n";
        exit(0);
    }
    return $path_info;
}

sub getFileData {
    my ($q, $fieldName) = @_;
    my $filename = $q->param($fieldName);
    unless ($filename) {
        print "Error: can't find file to upload<br>\n";
        exit(0);
    }
    my $uploadFilehandle = $q->upload($fieldName);
    return ($filename, $uploadFilehandle);
}

sub createUploadedFile {
    my ($filename, $uploadPath) = @_;
    my $safeFilename = basename($filename);
    my $filePath = $uploadPath . '/' . encode_entities($safeFilename);
    open(my $uploadedFile, '>', $filePath) or die "Cannot open file $filePath";
    return $uploadedFile;
}

sub copyFile {
    my ($fileHandle, $destFileHandle) = @_;
    while (my $chunk = <$fileHandle>) {
        print $destFileHandle $chunk;
    }
}

1;