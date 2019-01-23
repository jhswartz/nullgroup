# nullgroup

### Synopsis 
This program causes the denial of group permissions for appropriately owned
files.

### Implementation
The concept is currently implemented as a Loadable Kernel Module for Linux, and
requires that the kernel has been configured with "LIVEPATCH=y".

A replacement for the [in_group_p] function is provided as a live patch that
adds a check to determine if a file's group ownership matches a group id that
represents a "null" group.

If a match is found, then zero is returned to indicate that the current task
does not have adequate group credentials to access the file.

### Preparation
```
root@somewhere# groupadd -g 65533 null
```

### Building
```
user@somewhere$ make
```

### Loading
```
root@somewhere# insmod nullgroup.ko
```

### Deactivation
```
root@somewhere# echo 0 > /sys/kernel/livepatch/nullgroup/enabled
```

### Testing
```
# Deactivate nullgroup patch.
root@somewhere# echo 0 > /sys/kernel/livepatch/nullgroup/enabled

# Create a test file.
root@somewhere# echo 'Discount Furnishers' > /tmp/towntalk

# Change the test file's group ownership to "null".
root@somewhere# chgrp null /tmp/towntalk

# Set permissions that, under normal circumstances, would allow:
#   the user that owns the test file to read from it or write to it;
#   the group that owns the test file to read from it;
#   and others to have no access to the test file at all.
root@somewhere# chmod 640 /tmp/towntalk

# Verify the test file's ownership and permissions.
root@somewhere# ls -al /tmp/towntalk
-rw-r----- 1 root null 21 Oct 18 13:44 /tmp/towntalk

# Add a user to the "null" group".
root@somewhere# vipw -g
null:x:65533:user

# Ensure that the user has "null" group access.
user@somewhere$ id
uid=1000(user) gid=1000(user) groups=1000(user),65533(null)

# Attempt to read from the test file.
user@somewhere$ cat /tmp/towntalk
Discount Furnishers

# Activate nullgroup patch.
root@somewhere# echo 1 > /sys/kernel/livepatch/nullgroup/enabled

# Attempt to read from the test file, again.
user@somewhere$ cat /tmp/towntalk
cat: /tmp/towntalk: Permission denied
```

### Warning
Use this software at your own peril!

### Credit 
The concept was shared by [Bernd Jendrissek].

[in_group_p]: https://github.com/torvalds/linux/blob/v4.2/kernel/groups.c#L255
[Bernd Jendrissek]: http://www.bpj-code.co.za/
