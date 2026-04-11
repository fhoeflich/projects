#!/usr/bin/env pythonpm
#
# deploy-ova(8) - Deploy a VM from an OVA file containing properties etc.
#
import time
import threading
import os
import urllib2
import mmap
from urlparse import urlparse

from pysphere import VIServer, VIProperty
from pysphere.resources import VimService_services as VI

def get_descriptor(ovf_path):
    fh = open(ovf_path, "r")
    ovf_descriptor = fh.read()
    fh.close()
    return ovf_descriptor

def parse_descriptor(ovf_descriptor):
    ovf_manager = s._do_service_content.OvfManager
    request = VI.ParseDescriptorRequestMsg()
    _this =request.new__this(ovf_manager)
    _this.set_attribute_type(ovf_manager.get_attribute_type())
    request.set_element__this(_this)
    request.set_element_ovfDescriptor(ovf_descriptor)
    pdp = request.new_pdp()
    pdp.set_element_locale("")
    pdp.set_element_deploymentOption("")
    request.set_element_pdp(pdp)
    return s._proxy.ParseDescriptor(request)._returnval

def validate_host(host, ovf_descriptor):
    ovf_manager = s._do_service_content.OvfManager
    request = VI.ValidateHostRequestMsg()
    _this =request.new__this(ovf_manager)
    _this.set_attribute_type(ovf_manager.get_attribute_type())
    request.set_element__this(_this)
    request.set_element_ovfDescriptor(ovf_descriptor)
    h = request.new_host(host)
    h.set_attribute_type(host.get_attribute_type())
    request.set_element_host(h)
    vhp = request.new_vhp()
    vhp.set_element_locale("")
    vhp.set_element_deploymentOption("")
    request.set_element_vhp(vhp)
    return s._proxy.ValidateHost(request)._returnval

def find_network_by_name(network_name):
    ret = None
    for dc in  s.get_datacenters().keys():
        dc_properties = VIProperty(s, dc)
        for nw in dc_properties.network:
            if nw.name == network_name:
                ret = nw._obj
                break
        if ret:
            break
    if not ret:
        raise ValueError("Couldn't find network '%s'" % (network_name))
    return ret

def find_vmfolder_by_name(folder_name):
    ret = None
    for dc in s.get_datacenters().keys():
        dc_properties = VIProperty(s, dc)
        if dc_properties.vmFolder.name == folder_name:
            return dc_properties.vmFolder._obj
    raise ValueError("Couldn't find folder '%s'" % (folder_name))

def create_import_spec(resource_pool_mor,
                       datastore_mor,
                       ovf_descriptor,
                       name,
                       host=None,
                       network_mapping=None,
                       ip_allocation_policy="fixedPolicy",
                       ip_protocol="IPv4",
                       disk_provisioning="flat"
                       ):
    #get the network MORs:
    networks = {}
    if network_mapping:
        for ovf_net_name, vmware_net_name in network_mapping.items():
            networks[ovf_net_name] = find_network_by_name(vmware_net_name)

    ovf_manager = s._do_service_content.OvfManager
    request = VI.CreateImportSpecRequestMsg()
    _this =request.new__this(ovf_manager)
    _this.set_attribute_type(ovf_manager.get_attribute_type())
    request.set_element__this(_this)
    request.set_element_ovfDescriptor(ovf_descriptor)
    rp = request.new_resourcePool(resource_pool_mor)
    rp.set_attribute_type(resource_pool_mor.get_attribute_type())
    request.set_element_resourcePool(rp)
    ds = request.new_datastore(datastore_mor)
    ds.set_attribute_type(datastore_mor.get_attribute_type())
    request.set_element_datastore(ds)
    cisp = request.new_cisp()
    cisp.set_element_entityName(name)
    cisp.set_element_locale("")
    cisp.set_element_deploymentOption("")
    if host:
        h = cisp.new_hostSystem(host)
        h.set_attribute_type(host.get_attribute_type())
        cisp.set_element_hostSystem(h)
    if networks:
        networks_map = []
        for ovf_net_name, net_mor in networks.items():
            network_mapping = cisp.new_networkMapping()
            network_mapping.set_element_name(ovf_net_name)
            n_mor = network_mapping.new_network(net_mor)
            n_mor.set_attribute_type(net_mor.get_attribute_type())
            network_mapping.set_element_network(n_mor)
            networks_map.append(network_mapping)
#        cisp.set_element_networkMapping(network_mapping)
        cisp.set_element_networkMapping(networks_map)
    if ip_allocation_policy:
        cisp.set_element_ipAllocationPolicy(ip_allocation_policy)
    if ip_protocol:
        cisp.set_element_ipProtocol(ip_protocol)
    if disk_provisioning:
        cisp.set_element_diskProvisioning(disk_provisioning)
    request.set_element_cisp(cisp)
    return s._proxy.CreateImportSpec(request)._returnval

def import_vapp(resource_pool, import_spec, host=None, folder=None):
    #get the vm folder MOR
    if folder:
        folder = find_vmfolder_by_name(folder)
    request = VI.ImportVAppRequestMsg()
    _this =request.new__this(resource_pool)
    _this.set_attribute_type(resource_pool.get_attribute_type())
    request.set_element__this(_this)
    request.set_element_spec(import_spec.ImportSpec)
    if host:
        h = request.new_host(host)
        h.set_attribute_type(host.get_attribute_type())
        request.set_element_host(h)
    if folder:
        f = request.new_folder(folder)
        f.set_attribute_type(folder.get_attribute_type())
        request.set_element_folder(f)
    return s._proxy.ImportVApp(request)._returnval

def keep_lease_alive(lease):
    request = VI.HttpNfcLeaseProgressRequestMsg()
    _this =request.new__this(lease)
    _this.set_attribute_type(lease.get_attribute_type())
    request.set_element__this(_this)
    request.set_element_percent(50) #then we can add logic to set a real one
    while go_on:
        s._proxy.HttpNfcLeaseProgress(request)
        time.sleep(5)

go_on = True

if __name__ == "__main__":
    #you can get the resource pools running s.get_resource_pools()
    RESOURCE_POOL = "/Resources"
#    OVF_FILE = r"C:\FreeSCO\FreeSCO-043.ovf"
#    OVF_FILE = r"/var/tmp/recorder-manager-v3.2.1-36.ovf"
    OVF_FILE = r"/var/tmp/service-manager.ovf"
    #you can get the host names running s.get_hosts()
#    HOST = "localhost.localdomain"
    HOST = "35.1.8.2"
#    HOST = "107.0.14.58"
#    DATASTORE = "datastore1"
    DATASTORE = "datastore"
#    NETWORK_MAPPING = {"OVF Network Name":"VMWare Network Name", "OVF2":"VMWare 2"}
    NETWORK_MAPPING = {"VM Network":"VM Network", "MOS Internal":"VM Network"}
#    VAPP_NAME = "My New VM2"
    VAPP_NAME = "fhoeflic-sm"
    s = VIServer()
#    s.connect("host", "username", "password")
#    s.connect("35.1.8.2", "root", "cisco123", trace_file='debug.txt')
    s.connect("35.1.5.21", "root", "cisco123", trace_file='debug.txt')

    try:
        host = [k for k,v in s.get_hosts().items() if v==HOST][0]

        resource_pool = [k for k,v in s.get_resource_pools().items()
                         if v == RESOURCE_POOL][0]
        datastore = [k for k,v in s.get_datastores().items() if v==DATASTORE][0]

        ovf = get_descriptor(OVF_FILE)
        descriptor_info =  parse_descriptor(ovf)
        support_info = validate_host(host, ovf)

        import_spec = create_import_spec(resource_pool,
                                         datastore,
                                         ovf,
                                         VAPP_NAME,
                                         host=host,
                                         network_mapping=NETWORK_MAPPING,
                                         )
        if hasattr(import_spec, "Warning"):
            print "Warning:", import_spec.Warning[0].LocalizedMessage
        if hasattr(import_spec, "Error"):
            print "Error:", import_spec.Error[0].LocalizedMessage
        s.disconnect()
        exit()
#***----*** Done to here ***----***

        http_nfc_lease = import_vapp(resource_pool, import_spec, host=host)

        lease = VIProperty(s, http_nfc_lease)
        while lease.state == 'initializing':
            print lease.state
            lease._flush_cache()
        if lease.state != 'ready':
            print "something went wrong"
            exit()

        t = threading.Thread(target=keep_lease_alive, args=(http_nfc_lease,))
        t.start()
        for dev_url in lease.info.deviceUrl:
            filename = dev_url.targetId
            hostname = urlparse(s._proxy.binding.url).hostname
            upload_url = dev_url.ulr.replace("*", hostname)
            filename = os.path.join(os.path.dirname(OVF_FILE), filename)
            fsize = os.stat(filename).st_size
            f = open(filename,'rb')
            mmapped_file = mmap.mmap(f.fileno(), 0, access=mmap.ACCESS_READ)
            request = urllib2.Request(upload_url, upload_url=mmapped_file)

            request.add_header("Content-Type", "application/x-vnd.vmware-streamVmdk")
            request.add_header("Connection", "Keep-Alive")
            request.add_header("Content-Length", str(fsize))
            opener = urllib2.build_opener(urllib2.HTTPHandler)
            resp = opener.open(request)
            mmapped_file.close()
            f.close()

        go_on = False
        t.join()
        request = VI.HttpNfcLeaseCompleteRequestMsg()
        _this =request.new__this(http_nfc_lease)
        _this.set_attribute_type(http_nfc_lease.get_attribute_type())
        request.set_element__this(_this)
        s._proxy.HttpNfcLeaseComplete(request)

    finally:
        s.disconnect()
