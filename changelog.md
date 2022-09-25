# v1.3
 - Added support for IO operations up to at least 28 GB (up from 4 KB)
    - Larger IO operations may work, but keep in mind that the memory is duplicated,
        so you will need at least twice the amount of memory you want to access
 Note: C wrapper v1.6 will continue work with this version

# v1.2.2
 - First release working version, supports IO operations up to 4 KB
 - Removed debug messages which can fill the disk with the dmesg buffer
 Works best with C wrapper v1.6
