.section .data
.global initrd_start, initrd_end

initrd_start:
.incbin "build/ramdisk.img"
initrd_end: