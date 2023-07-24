global outb
global inb

outb:
    mov al, sil
    mov dx, di
    out dx, al
    ret

inb:
    mov dx, di
    in  al, dx
    ret