# Hey! I'm Filing Here

In this lab, I successfully implemented the creation of a 1 MiB ext2 file system. My implementation involved constructing a valid ext2 filesystem image and ensuring that the kernel could mount it. I filled out the superblock, block group descriptor table, block and inode bitmaps, and the inode table. Additionally, I created the root directory and the required files, including 'lost+found,' 'hello-world,' and a symbolic link named 'hello' that pointed to 'hello-world.' All permissions, ownerships, and file attributes were accurately set as per the provided specifications.

## Building

```shell
make
```

## Running

```shell
./ext2-create # run the executable to create cs111-base.img
mkdir mnt # create a directory to mount the file system to
sudo mount -o loop cs111-base.img mnt # mount filesystem to directory mnt
```

## Cleaning up

```shell
sudo umount mnt # unmount the filesystem
rmdir mnt # delete the directory used for mounting
make clean
```
