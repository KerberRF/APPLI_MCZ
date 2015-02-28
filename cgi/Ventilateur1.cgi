#!/usr/bin/perl -w
     # Tell perl to send a html header.
     # So your browser gets the output
     # rather then <stdout>(command line
     # on the server.)
use CGI qw(:standard);
use strict;
use XML::LibXML;

 my $query = new CGI;
 my $valeur_vent1 = $query->param('Vent1'); #donnee d'entree "Vent1"
 my $filename = "parameters.xml";
 my $p = XML::LibXML->new();
 my $d = $p->parse_file($filename);

for my $node ($d->findnodes('//ParametersOfMCZ'))
{
   my ($vent1_node) = $node->findnodes('Ventilateur1/text()')
      or next;
   $vent1_node->setData($valeur_vent1);
}
print $d->toString; #Affiche
print $d->toFile($filename,0);

