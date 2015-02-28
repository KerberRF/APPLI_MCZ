#!/usr/bin/perl -w
     # Tell perl to send a html header.
     # So your browser gets the output
     # rather then <stdout>(command line
     # on the server.)
system "/home/pi/Dev/APPLIMCZ/src/./Appli_Emit";

print "Content-type: text/html\n\n";
     # print your basic html tags.
     # and the content of them.
print "<html><head><title>Hello World!! </title></head>\n";
print "<body><h1>Hello world</h1></body></html>\n";
