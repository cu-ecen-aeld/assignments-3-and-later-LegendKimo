#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aesd-autograder
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v5.15.163
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}

    # TODO: Add your kernel build steps here
    export PATH=$PATH:/usr/aarch64-linux-gnu/bin
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- mrproper
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- defconfig
    make -j4 ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules
    make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- dtbs
    
fi

echo "Adding the Image in outdir"

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories
mkdir ./rootfs
cd ./rootfs
mkdir bin dev etc home lib proc sbin sys tmp usr var
mkdir usr/bin usr/lib usr/sbin
mkdir -p var/log
mkdir -p home/conf

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
	make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-  defconfig
else
    cd busybox
fi

# TODO: Make and install busybox
make CONFIG_PREFIX=../rootfs ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- install
cd "$OUTDIR"/rootfs/
echo "Library dependencies"
${CROSS_COMPILE}readelf -a bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a bin/busybox | grep "Shared library"

# TODO: Add library dependencies to rootfs
# cp -a /usr/arm-linux-gnueabihf/lib/ld-linux-armhf.so.3 ~/embedded-linux-qemu-labs/tinysystem/nfsroot/lib/
# cp -a /usr/arm-linux-gnueabihf/lib/libm.so.6 ~/embedded-linux-qemu-labs/tinysystem/nfsroot/lib/
# cp -a /usr/arm-linux-gnueabihf/lib/libresolv.so.2 ~/embedded-linux-qemu-labs/tinysystem/nfsroot/lib/
# cp -a /usr/arm-linux-gnueabihf/lib/libc.so.6  ~/embedded-linux-qemu-labs/tinysystem/nfsroot/lib/
cp -a /usr/aarch64-linux-gnu/lib/libm.so.6       ./lib/
cp -a /usr/aarch64-linux-gnu/lib/libresolv.so.2 ./lib/
cp -a /usr/aarch64-linux-gnu/lib/libc.so.6 ./lib/
cp -a /usr/aarch64-linux-gnu/lib/ld-linux-aarch64.so.1 ./lib/

# TODO: Make device nodes
sudo mknod -m 666 dev/null c 1 3
sudo mknod -m 600 dev/console c 5 1

# TODO: Clean and build the writer utility
ls -al "${SCRIPT_DIR}/"
cd "${SCRIPT_DIR}/"
make clean
make CROSS_COMPILE=aarch64-linux-gnu- writer
# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
#cd "$OUTDIR"
cp ./writer "$OUTDIR/rootfs/home"
cp ./finder.sh "$OUTDIR/rootfs/home"
cp ./finder-test.sh "$OUTDIR/rootfs/home"
cp ./autorun-qemu.sh "$OUTDIR/rootfs/home"
cp ./conf/username.txt "$OUTDIR/rootfs/home/conf"
cp ./conf/assignment.txt "$OUTDIR/rootfs/home/conf"
cp ./writer.sh "$OUTDIR/rootfs/home"
# TODO: Chown the root  directory
cd "$OUTDIR/rootfs/"
sudo chown -R root:root *

# TODO: Create initramfs.cpio.gz
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
gzip -f ${OUTDIR}/initramfs.cpio
cp ${OUTDIR}/linux-stable/arch/arm64/boot/Image ${OUTDIR}/
