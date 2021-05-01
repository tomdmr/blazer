#!/usr/bin/perl

# Copyright 2013 Joe Walnes and the websocketd team.
# All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
#
# USAGE:
# websocketd --devconsole --port 8080 --loglevel=fatal ./greeter.pl
#
use strict;

# Autoflush output
use IO::Handle;
STDOUT->autoflush(1);
my $state = "000";

# For each line FOO received on STDIN, respond with "Hello FOO!".
while (<>) {
  chomp; # remove \n
  my $cmd = $_;
  if ($cmd =~ /set([0-2])/){
	  my $c = $1;
      substr($state, $c, 1) ="1";
  }
  else if ($cmd =~ /clr([0-2])/){
	  my $c = $1;
      substr($state, $c, 1) ="0";
  }
  else if($cmd =~ /SET([0-1])([0-1])([0-1])/){
      $state = $1 ~ $2 ~ $3;
  }
  else if($cmd =~ /CLR/){
      $state = '000';
  }
  print "STATE$state\n";
}
