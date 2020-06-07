#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <arch/i386/ps2.h>
#include <arch/i386/irq.h>

#include <sys/io.h>
#include <sys/debug.h>
#include <sys/kb.h>
#include <sys/shell.h>

#define LCTL 0x10
#define LSHF 0x11
#define LALT 0x12
#define RCTL LCTL
#define RSHF 0x14 
#define RALT LALT

// Different keyset
#define KS_DEFAULT 0
#define KS_SHIFT   1

struct {
    bool lshift;
    bool rshift;
    bool lctrl;
    bool rctrl;
    bool lalt;
    bool ralt;
} ps2kb_state;

char (*kb_map)[2][256];

char gb[2][256] = { // GB keyboard mapping
    {
        /* Unmodified keyset
           0   1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  */
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,'\t', 0  , 0  , // 0
        0  ,LALT,LSHF, 0  ,LCTL, 'q', '1', 0  , 0  , 0  , 'z', 's', 'a', 'w', '2', 0  , // 1
        0  , 'c', 'x', 'd', 'e', '4', '3', 0  , 0  , ' ', 'v', 'f', 't', 'r', '5', 0  , // 2
        0  , 'n', 'b', 'h', 'g', 'y', '6', 0  , 0  , 0  , 'm', 'j', 'u', '7', '8', 0  , // 3
        0  , ',', 'k', 'i', 'o', '0', '9', 0  , 0  , '.', '/', 'l', ';', 'p', '-', 0  , // 4
        0  , 0  ,'\'', 0  , '[', '=', 0  , 0  , 0  ,RSHF,'\n', ']', 0  , '#', 0  , 0  , // 5
        0  ,'\\', 0  , 0  , 0  , 0  ,'\b', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 6
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 7
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 8
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 9
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // A
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // B
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // C
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // D
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // E
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0    // F
    },
    {
        /* SHIFT held down
           0   1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  */
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,'\t', 0  , 0  , // 0
        0  ,LALT,LSHF, 0  ,LCTL, 'Q', '!', 0  , 0  , 0  , 'Z', 'S', 'A', 'W', '"', 0  , // 1
        0  , 'C', 'X', 'D', 'E', '$', '£', 0  , 0  , ' ', 'V', 'F', 'T', 'R', '%', 0  , // 2
        0  , 'N', 'B', 'H', 'G', 'Y', '^', 0  , 0  , 0  , 'M', 'J', 'U', '&', '*', 0  , // 3
        0  , '<', 'K', 'I', 'O', ')', '(', 0  , 0  , '>', '?', 'L', ':', 'P', '_', 0  , // 4
        0  , 0  ,'@' , 0  , '{', '+', 0  , 0  , 0  ,RSHF,'\n', '}', 0  , '~', 0  , 0  , // 5
        0  ,'|' , 0  , 0  , 0  , 0  ,'\b', 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 6
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 7
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 8
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 9
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // A
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // B
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // C
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // D
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // E
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0    // F
    }
};

void ps2kb_handler(struct regs *r)
{
    uint8_t scancode = 0;
    scancode = ps2_read();

    struct key_event e;
    e.flags = 0x1; // Set special bit by default
    bool state;
    if(scancode == 0xF0)
    {
        state = false;
        scancode = ps2_read();
    }
    else if(scancode == 0xE0)
    {
        scancode = ps2_read();
        if(scancode == 0xF0)
        {
            state = false;
            scancode = ps2_read();
        }
        else
        {
            state = true;
        }
        switch((*kb_map)[KS_DEFAULT][scancode])
        {
            case RCTL:
                ps2kb_state.rctrl = state;
                break;
            case RALT:
                ps2kb_state.ralt  = state;
                break;
            default:
                printf("Unhandled E0 scancode %x", scancode);
                break;
        }
    }
    else if(scancode == 0)
    {
        printf("WHY 0?");
    }
    else
    {
        state = true;
    }

    switch((*kb_map)[KS_DEFAULT][scancode])
    {
        case 0:
            printf("<%x:%d>", scancode,__LINE__);
            break;
        case LSHF:
            ps2kb_state.lshift = state;
            break;
        case LCTL:
            ps2kb_state.lctrl  = state;
            break;
        case LALT:
            ps2kb_state.lalt   = state;
            break;
        case RSHF:
            ps2kb_state.rshift = state;
            break;
        default:
            // Find utf-8 equivalent if not modified
            e.flags = 0 + // Special bit unset, i.e. character
                (((int)(ps2kb_state.lctrl || ps2kb_state.rctrl)) << 1) +
                (((int)(ps2kb_state.lshift || ps2kb_state.rshift)) << 2) +
                (((int)(ps2kb_state.lalt || ps2kb_state.ralt )) << 3) +
                ((int)state << 7);
            if (e.flags & KB_FLAG_CONTROL || e.flags & KB_FLAG_ALT)
            {
                e.utf8 = 0;
                e.flags &= KB_FLAG_SPECIAL;
            }
            else if (e.flags & KB_FLAG_SHIFT)
            {
                e.utf8 = (*kb_map)[KS_SHIFT][scancode];
            }
            else
            {
                e.utf8 = (*kb_map)[KS_DEFAULT][scancode];
            }
            break;
    }

    shell_key(e);
    // Fire key_event ==> >=3 }-->

    //  shell_input_key_event(e);

    serial_writes("KB:");
    serial_writed(e.utf8);
    serial_writes("\n");
    // THERE'S SOMETHING WRONG HERE
    }

void ps2kb_init(void)
{
    //inb(0x60);
    irq_install_handler(1, ps2kb_handler);

    ps2kb_state.lshift = false;
    ps2kb_state.rshift = false;
    ps2kb_state.lctrl  = false;
    ps2kb_state.rctrl  = false;
    ps2kb_state.lalt   = false;
    ps2kb_state.ralt   = false;

    kb_map = &gb;

    serial_writes("ps2 keyboard installed [WARNING! THIS REALLY DOESN't WORK]\n");
}

