%define _use_internal_dependency_generator 0

Name: bgq-distrofs_runtime
Summary: BG/Q Linux Distribution runtime files
Group: Software
Version: %{_product_version}
Release: %{_rpm_release}
License: IBM Corporation 2012
URL: http://www.ibm.com
Vendor: IBM Corporation
Prefix: / 

%description

Contains the scripts and binaries which are unique to BG/Q for installation 
into the BG/Q Linux Distribution. ( %_rpm_driver_lvl )

%install
rm -rf %{_topdir}/BUILDROOT/bgq-distrofs_runtime-%{_rpm_ver_rel}.%{_os_arch} ; ln -s %{_base_dir}/ramdisk/distrofs/rpm %{_topdir}/BUILDROOT/bgq-distrofs_runtime-%{_rpm_ver_rel}.%{_os_arch}

%files

%defattr(-,root,root,755)
/bgfs/
/bgsys/
/bin/
/etc/
/etc.mirror/
/lib64/
/root.mirror/
/usr/
/var.mirror/
