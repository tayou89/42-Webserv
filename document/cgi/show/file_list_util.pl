use strict;
use warnings;

sub makeHTMLFileString {
    my ($fileName) = @_;
    return qq(<li><a href="/show/file.py?file=$fileName">$fileName</a></li>\n);
}

sub getHTMLStringList {
    my (@fileList) = @_;
    my $htmlStringList = '';
    foreach my $file (@fileList) {
        $htmlStringList .= makeHTMLFileString($file);
    }
    return $htmlStringList;
}

1;
