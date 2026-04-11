#!/usr/bin/env pythonpm
#
# Ported from perl
#
# 1. Fix up imports to bring in pysphere's VIServer package and OptionParser
#use strict;
#use warnings;
#use VMware::VIRuntime;
#use VMware::VILib;
#use VMware::VIExt;
import os
import sys
from optparse import OptionParser
from pysphere import VIServer

# 2. Tool to fetch name of VM with MAC address `macaddr'.
def get_vm_by_mac(server, macaddr):
    props = server._retrieve_properties_traversal(property_names=["name"],obj_type="VirtualMachine")
    for obj in props:
        for prop in obj.PropSet:
                if prop.Name == 'name':
                    name = prop.Val
                    vm = server.get_vm_by_name(name)
#                    print "Found name " + name
                    net = vm.get_property('net', from_cache=False)
                    if net:
#                        print "Found net on " + name
                        for interface in net:
                            thismac = interface.get('mac_address', None)
                            if (thismac == macaddr):
#                                print "Got MAC: " + thismac + " on VM " + name
                                return name
                    for v in vm.get_property("devices").values():
                        devmac = v.get('macAddress')
                        if (devmac == macaddr):
#                           print "Got device MAC: " + devmac + " on VM " + name
                           return name

#my %opts = (
#    vihost => {
#        alias => "h",
#        type => "=s",
#        help => qq!    The host to use when connecting via a vCenter Server. !,
#        required => 0,
#    },
#);

# 3. Fetch any options that were passed in to us
# This script should really be called `get-vm-name-by-macaddr' or similar.
#Opts::add_options(%opts);
version = '1.0'
parser = OptionParser(usage="usage: %prog [options]", version="%prog " + version)
parser.add_option("--server",
                    action="store",
                    dest="host",
                    type="string",
                    metavar="HOSTNAME",
                    default=None,
                    help="hostname/IP of VMware target")
parser.add_option("--username",
                    action="store",
                    dest="username",
                    type="string",
                    metavar="USERNAME",
                    default=None,
                    help="VMware target user name")
parser.add_option("--password",
                    action="store",
                    dest="password",
                    type="string",
                    metavar="PASSWORD",
                    default=None,
                    help="VMware target password")
parser.add_option("--macaddr",
                    action="store",
                    dest="macaddr",
                    type="string",
                    metavar="MACADDR",
                    default=None,
                    help="MAC address of primary interface on VMware target")
parser.add_option("--vihost",
                    action="store",
                    dest="vihost",
                    type="string",
                    metavar="VIHOST",
                    default=None,
                    help="optional VMware ESX host instead of vCentre server")

(options, args) = parser.parse_args()
if options.vihost != None:
    host = options.vihost
else:
    host = options.host

# 4. Connect to the server
# validate options, and connect to the server
#Opts::parse();
#Opts::validate();
#Util::connect();
server = VIServer()
server.connect(host, options.username, options.password)
						# XXX: should try/except

#my $vm_view = Vim::find_entity_views(view_type => 'VirtualMachine');
 
#my $rec = `/sbin/ifconfig -a | grep "HWaddr"`;
#my ($field1, $field2, $field3, $field4, $macAddress) = split(/\s+/, $rec);

# 5. Find VM by macaddr and print its name
#FIND_VM: {
#    if($macAddress) {
#
#        foreach(@$vm_view) {
#            my $vm_name = $_->summary->config->name;
#            my $devices =$_->config->hardware->device;
#
#            foreach(@$devices) {
#                if($_->isa("VirtualEthernetCard")) {
#                    if($_->macAddress =~ /$macAddress/i) {
#                        print $vm_name . "\n";
#                        last FIND_VM;
#                    }
#                }
#            }
#        }
#    }
#}
#
print get_vm_by_mac(server, options.macaddr.lower())

# 6. Shut down server connection 
#Util::disconnect();
server.disconnect() 
