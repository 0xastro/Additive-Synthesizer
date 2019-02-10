#include "keyboard.h"


extern struct waves_buffer signal_params;


TASK keyboard_thread(void* arg){

    task_sepcs.id_keyboard=get_task_index(arg);
    task_sepcs.prio_keyboard=get_task_period(arg);

    ALLEGRO_EVENT           event;
    ALLEGRO_EVENT_QUEUE*    queue;

    queue = al_create_event_queue();

    signal_params.gain=2;

    al_register_event_source(queue, al_get_keyboard_event_source());

    set_activation(task_sepcs.id_keyboard);
    while(!TASK_END){

        if(LOG_ENABLE){
            printf("DEBUG\t\tKeyboard TASK\n");
            log_key(NULL,event.keyboard.keycode,event.keyboard.unichar,event.keyboard.modifiers);
        }

        // Actively poll the keyboard to trigger the waves notes
        WavesON_OFF();
        sin_notes();
        square_notes();
        triangle_notes();

        task_sepcs.dlmiss_keyboard=deadline_miss(task_sepcs.id_keyboard);
        wait_for_period(task_sepcs.id_keyboard);
    }
}




void log_key(char const *how, int keycode, int unichar, int modifiers){

    char multibyte[5] = {0, 0, 0, 0, 0};
    const char* key_name;
    al_utf8_encode(multibyte, unichar <= 32 ? ' ' : unichar);

    key_name = al_keycode_to_name(keycode);

    log_printf("%-8s  code=%d, char='%s' (%4d), modifiers=%08x, [%s]\n",
                how, keycode, multibyte, unichar, modifiers, key_name);
}



void sin_notes(void){

    al_get_keyboard_state(&keyState);

    if (al_key_down(&keyState, ALLEGRO_KEY_A))
        signal_params.freq_sin=NOTE_A;
    else if (al_key_down(&keyState, ALLEGRO_KEY_S))
        signal_params.freq_sin=NOTE_B;
    else if (al_key_down(&keyState, ALLEGRO_KEY_D))
        signal_params.freq_sin=NOTE_C;
    else if (al_key_down(&keyState, ALLEGRO_KEY_F))
        signal_params.freq_sin=NOTE_D;
    else if (al_key_down(&keyState, ALLEGRO_KEY_G))
        signal_params.freq_sin=NOTE_E;
    else if (al_key_down(&keyState, ALLEGRO_KEY_H))
        signal_params.freq_sin=NOTE_F;
    else if (al_key_down(&keyState, ALLEGRO_KEY_J))
        signal_params.freq_sin=NOTE_G;

}


void square_notes(void){

    al_get_keyboard_state(&keyState);

    if (al_key_down(&keyState, ALLEGRO_KEY_Z)){
        signal_params.freq_square=NOTE_A;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_X)){
        signal_params.freq_square=NOTE_B;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_C)){
        signal_params.freq_square=NOTE_C;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_V)){
        signal_params.freq_square=NOTE_D;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_B)){
        signal_params.freq_square=NOTE_E;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_N)){
        signal_params.freq_square=NOTE_F;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_M)){
        signal_params.freq_square=NOTE_G;
    }

}

void triangle_notes(void){

    al_get_keyboard_state(&keyState);

    if (al_key_down(&keyState, ALLEGRO_KEY_Q)){
        signal_params.freq_tri=NOTE_A;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_W)){
        signal_params.freq_tri=NOTE_B;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_E)){
        signal_params.freq_tri=NOTE_C;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_R)){
        signal_params.freq_tri=NOTE_D;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_T)){
        signal_params.freq_tri=NOTE_E;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_Y)){
        signal_params.freq_tri=NOTE_F;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_U)){
        signal_params.freq_tri=NOTE_G;
    }

}


void WavesON_OFF(void){

    if (al_key_down(&keyState, ALLEGRO_KEY_1) && signal_params.sine_switch == 1){
        signal_params.sine_switch=0;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_1) && signal_params.sine_switch == 0){
        signal_params.sine_switch=1;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_2) && signal_params.sine_switch == 1){
        signal_params.square_switch=0;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_2) && signal_params.sine_switch == 0){
        signal_params.square_switch=1;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_3) && signal_params.sine_switch == 1){
        signal_params.tri_switch=0;
    }
    else if (al_key_down(&keyState, ALLEGRO_KEY_3) && signal_params.sine_switch == 0){
        signal_params.tri_switch=1;
    }  

}
