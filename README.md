I had to go into /etc/ld.so.conf.d/
and add a file called "epics.conf" with the line,
root@fuze2:/etc/ld.so.conf.d# more epics.conf
/usr/src/epics/base-3.15.3/lib/linux-x86_64

otherwise, the OS couldn't find the shared library. When I ran the binary, I would get this message:
./pv_server: error while loading shared libraries: libdbCore.so.3.15.3: cannot open shared object file: No such file or directory

I installed the epics distribution in /usr/src/epics/base-3.15.3