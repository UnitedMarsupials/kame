# Makefile for syscall tables
#
# $FreeBSD: src/sys/alpha/linux/Makefile,v 1.5.2.1 2000/11/04 07:30:08 obrien Exp $

all:
	@echo "make sysent only"

sysent:  linux_sysent.c linux_syscall.h linux_proto.h

linux_sysent.c linux_syscall.h linux_proto.h: ../../kern/makesyscalls.sh \
		syscalls.master syscalls.conf
	-mv -f linux_sysent.c linux_sysent.c.bak
	-mv -f linux_syscall.h linux_syscall.h.bak
	-mv -f linux_proto.h linux_proto.h.bak
	sh ../../kern/makesyscalls.sh syscalls.master syscalls.conf
