#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <arch/i386/ps2.h>
#include <arch/i386/irq.h>

#include <sys/io.h>
#include <sys/debug.h>
#include <sys/kb.h>
#include <sys/shell.h>

#define LCTL 0x14
#define LALT 0x11
#define LSHF 0x12
#define RCTL LCTL
#define RSHF 0x59
#define RALT LALT

#define MOD  0x1F

#define KUA  KEY_UP_ARROW
#define KDA  KEY_DOWN_ARROW
#define KRA  KEY_RIGHT_ARROW
#define KLA  KEY_LEFT_ARROW
#define KPU  KEY_PAGE_UP
#define KPD  KEY_PAGE_DOWN

#define F1  0x05
#define F2  0x06
#define F3  0x04
#define F4  0x0C
#define F5  0x03
#define F6  0x0B
#define F7  0x83
#define F8  0x0A // 0x0A conflicted with \n
#define F9  0x01
#define F10 0x09
#define F11 0x78
#define F12 0x07

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
    bool mod;
} ps2kb_state;

unsigned char (*kb_map)[2][256];

unsigned char gb[2][256] = { // GB keyboard mapping
    {
        /* Unmodified keyset
         0   1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  */
        0  , F9 , 0  , F5 , F3 , F1 , F2 , F12, 0  , F10, F8 , F6 , F4 ,'\t', '`', 0  , // 0
        0  ,LALT,LSHF, 0  ,LCTL, 'q', '1', 0  , 0  , 0  , 'z', 's', 'a', 'w', '2', MOD, // 1
        0  , 'c', 'x', 'd', 'e', '4', '3', 0  , 0  , ' ', 'v', 'f', 't', 'r', '5', 0  , // 2
        0  , 'n', 'b', 'h', 'g', 'y', '6', 0  , 0  , 0  , 'm', 'j', 'u', '7', '8', 0  , // 3
        0  , ',', 'k', 'i', 'o', '0', '9', 0  , 0  , '.', '/', 'l', ';', 'p', '-', 0  , // 4
        0  , 0  ,'\'', 0  , '[', '=', 0  , 0  , 0  ,RSHF,'\n', ']', 0  , '#', 0  , 0  , // 5
        0  ,'\\', 0  , 0  , 0  , 0  ,'\b', 0  , 0  , 0  , 0  , KLA, 0  , 0  , 0  , 0  , // 6
        0  , 0  , KDA, 0  , KRA, KUA, 0  , 0  , F11, 0  , KPD, 0  , 0  , KPU, 0  , 0  , // 7
        0  , 0  , 0  , F7 , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , // 8
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
        0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  , 0  ,'\t', '¬', 0  , // 0
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

    struct key_event e = {0, KEY_NONE, KB_FLAG_SPECIAL};
    bool state = true; // state { true == down, false == up }
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
        switch(scancode)//(*kb_map)[KS_DEFAULT][scancode])
        {
            case RCTL:
                ps2kb_state.rctrl = state;
                break;
            case RALT:
                ps2kb_state.ralt  = state;
                break;
            case MOD:
                ps2kb_state.mod = state;
                break;
            case 0:
                printf("<Unhandled E0 scancode %x>", scancode);
                break;
            default:
                e.code = scancode;
                break;
        }
    }
    else if(scancode == 0)
    {
        printf("0 scancode rx");
    }
    else
    {
        state = true;
    }

    switch(scancode)
    {
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
        case MOD:
            ps2kb_state.mod = state;
            break;
        case F1 :
        case F2 :
        case F3 :
        case F4 :
        case F5 :
        case F6 :
        case F7 :
        case F8 :
        case F9 :
        case F10:
        case F11:
        case F12:
            // TODO: do something with function keys
            break;
        default:
            switch((*kb_map)[KS_DEFAULT][scancode])
            {
                case 0:
                    printf("<Unmapped scancode %x>", scancode);
                    break;
                default:
                    // Find utf-8 equivalent if not modified
                    e.flags |= 
                        (((int)(ps2kb_state.lctrl || ps2kb_state.rctrl)) << 1) +
                        (((int)(ps2kb_state.lshift || ps2kb_state.rshift)) << 2) +
                        (((int)(ps2kb_state.lalt || ps2kb_state.ralt )) << 3) +
                        ((int)ps2kb_state.mod << 4) +
                        ((int)state << 7);
                    if (e.flags & KB_FLAG_CONTROL || e.flags & KB_FLAG_ALT || e.flags & KB_FLAG_MOD)
                    {
                        // These keys send modified letters
                        e.utf8 = (*kb_map)[KS_DEFAULT][scancode];
                        e.flags |= KB_FLAG_SPECIAL;
                    }
                    else if (e.code != KEY_NONE)
                    {
                        // For non-printable keys (arrows, etc.) with code set
                        e.utf8 = 0;
                        e.flags |= KB_FLAG_SPECIAL;
                    }
                    else if (e.flags & KB_FLAG_SHIFT)
                    {
                        e.utf8 = (*kb_map)[KS_SHIFT][scancode];
                        e.flags &= ~KB_FLAG_SPECIAL;
                    }
                    else
                    {
                        e.utf8 = (*kb_map)[KS_DEFAULT][scancode];
                        e.flags &= ~KB_FLAG_SPECIAL;
                    }
                    break;
             }
            break;
    }

    e.code=scancode;
    //printf("%d:<%c(%x):%x:%x>\n",__LINE__,e.utf8,e.utf8,e.code,e.flags);

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

    // TODO: determine cause of delay before output
    serial_writes("ps2 keyboard installed [WARNING! THIS REALLY DOESN't WORK]\n");
}

