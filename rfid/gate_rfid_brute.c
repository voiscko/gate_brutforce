#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lib/lfrfid/lfrfid_worker.h>
#include <lib/lfrfid/protocols/lfrfid_protocols.h>

typedef struct {
    LFRFIDWorker* worker;
    ProtocolDict* protocol_dict;
    uint32_t id;
    bool running;
} State;

static void draw(Canvas* c, void* ctx){
    State* st = ctx;
    canvas_clear(c);
    canvas_set_font(c, FontPrimary);
    canvas_draw_str(c,0,10,"RFID brute");
    char buf[32];
    snprintf(buf,sizeof(buf),"ID %08lX",st->id);
    canvas_draw_str(c,0,25,buf);
    canvas_draw_str(c,0,38,"OK = exit");
}

static void input(InputEvent* e, void* ctx){
    State* st = ctx;
    if(e->type==InputTypeShort && e->key==InputKeyOk) st->running=false;
}

static int32_t loop(void* p){
    State* st = p;
    while(st->running){
        // Start emulating with current ID
        lfrfid_worker_emulate_start(st->worker, LFRFIDProtocolEM4100);
        furi_delay_ms(100);  // Emulate for 100ms
        lfrfid_worker_stop(st->worker);
        
        st->id++;
        furi_delay_ms(20);   // Brief pause between IDs
    }
    return 0;
}

int32_t gate_rfid_brute_app(void* p){
    UNUSED(p);
    
    // Initialize protocol dictionary
    ProtocolDict* protocol_dict = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);
    
    State st = { 
        .worker = lfrfid_worker_alloc(protocol_dict), 
        .protocol_dict = protocol_dict,
        .id = 0, 
        .running = true 
    };
    
    ViewPort* vp = view_port_alloc();
    view_port_draw_callback_set(vp, draw, &st);
    view_port_input_callback_set(vp, input, &st);
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, vp, GuiLayerFullscreen);
    FuriThread* t = furi_thread_alloc_ex("b",2048,loop,&st);
    furi_thread_start(t);
    while(st.running){ view_port_update(vp); furi_delay_ms(100); }
    furi_thread_join(t);
    furi_thread_free(t);
    gui_remove_view_port(gui, vp);
    view_port_free(vp);
    lfrfid_worker_free(st.worker);
    protocol_dict_free(st.protocol_dict);
    furi_record_close(RECORD_GUI);
    return 0;
}
