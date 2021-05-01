#!/usr/bin/perl

# Copyright 2013 Joe Walnes and the websocketd team.
# All rights reserved.
# Use of this source code is governed by a BSD-style
# license that can be found in the LICENSE file.
#
# USAGE:
# websocketd --devconsole --port 8080 --loglevel=fatal ./greeter.pl
# Emulator for a real ESP32.
# It understands the verbs
#   setX, sets an LED, where X is the channel 0-2
#   clrX, clears an LED
#   SETaaa, where abc set (a=1) or clear (a=0) an LED
#   CLR,  unset al LEDs
#
use strict;

# Autoflush output
use IO::Handle;
STDOUT->autoflush(1);
my $state = "000";
print STDERR "Startup\n";

while (<>) {
    print STDERR "Got request\n";
  chomp; # remove \n
  my $cmd = $_;
  if ($cmd =~ /set([0-2])/){
	  my $c = $1;
      substr($state, $c, 1) ="1";
  }
  if ($cmd =~ /clr([0-2])/){
	  my $c = $1;
      substr($state, $c, 1) ="0";
  }
  if($cmd =~ /SET([0-1])([0-1])([0-1])/){
      $state = $1 . $2 . $3;
  }
  if($cmd =~ /CLR/){
      $state = '000';
  }
  print "STATE$state\n";
  print STDERR "STATE$state\n";
}
