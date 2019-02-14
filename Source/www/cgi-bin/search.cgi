#!/usr/bin/perl
##!C:/Perl/bin/perl.exe
use  LWP::Simple;
use CGI;
use URI::Escape;
use Encode;

$cgi = new CGI;




my $Query;
for $key ( $cgi->param() ) {
	$Query .= $key.'='.$cgi->param($key).'&';
	$QueryEsc .= $key.'='.uri_escape($cgi->param($key)).'&'; 

}


my $doc = get "http://txt.iphil.ru/cgi-bin/search?$QueryEsc";
print "Content-type: text/html; charset=windows-1251\n\n";

#print "http://txt.iphil.ru/cgi-bin/search?$QueryEsc";
#print "<br>";
#print "http://txt.iphil.ru/cgi-bin/search?$Query";
#print "<br>";

print encode('windows-1251',$doc); ;

