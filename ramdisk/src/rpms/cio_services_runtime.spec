%define _use_internal_dependency_generator 0

Name: bgq-cio_services_runtime
Summary: BG/Q CIO Services Runtime Binaries
Group: Software 
Version: %{_product_version}
Release: %{_rpm_release}
License: IBM Corporation 2012
URL: http://www.ibm.com
Vendor: IBM Corporation
Prefix: / 

%description

Contains the scripts, binaries. libraries and config files to support CIO Services 
in the BG/Q Linux Distribution. ( %_rpm_driver_lvl )

%install
rm -rf %{_topdir}/BUILDROOT/bgq-cio_services_runtime-%{_rpm_ver_rel}.%{_os_arch} ; ln -s %{_base_dir}/ramdisk/distrofs/cios %{_topdir}/BUILDROOT/bgq-cio_services_runtime-%{_rpm_ver_rel}.%{_os_arch}

%files

%defattr(-,root,root,755)
/etc/
/sbin/
/lib/
/lib64/
/jobs/
/var/

