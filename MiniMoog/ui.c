
#include "ui.h"

 
/*---------------------------------------------------------------------------------------------------------*/

TASK allegro_draw(void* arg) {

    task_sepcs.id_display=get_task_index(arg);
    task_sepcs.prio_display=get_task_period(arg);


    /*Defining Colors used in the program*/
    c0 = al_map_rgb(BLACK);
    c1 = al_map_rgb(PINC);
    c2 = al_map_rgb(WHITE);
    c3 = al_map_rgb_f(ELECTRIC);

    display = al_create_display(XDISP, YDISP);  
    font = al_load_font("data/DejaVuSans.ttf", 12, 1);
    
    set_activation(task_sepcs.id_display);
    
    while(!TASK_END){
        
        if (LOG_ENABLE) {
            log_printf("DEBUG\t\tSTART UI\n");
            log_printf("DEBUG:\t\t Display Deadline Miss:%d\n\n",task_sepcs.dlmiss_display);
        }

        Draw_Task_Bounded_Boxes();
        DrawTask_Info();
        PlotWaveforms();
        Draw_Instructions();
        al_flip_display();    
        
        task_sepcs.dlmiss_display=deadline_miss(task_sepcs.id_display);
        
        wait_for_period(task_sepcs.id_display);
    }
}


/*---------------------------------------------------------------------------------------------------------*/

void DrawTask_Info(void){
    
    al_draw_textf(font, c2, 14, 20, 0, "TASK AUDIO");
    al_draw_textf(font, c2, 14, 40, 0, "ID:            %d",task_sepcs.id_audio);
    al_draw_textf(font, c2, 14, 60, 0, "Priority:          %d",task_sepcs.prio_audio);
    al_draw_textf(font, c2, 14, 80, 0, "Deadline Miss:    %d",task_sepcs.dlmiss_audio);

    al_draw_textf(font, c2, 14+180, 20, 0, "TASK DISPLAY");
    al_draw_textf(font, c2, 14+180, 40, 0, "ID:            %d",task_sepcs.id_display);
    al_draw_textf(font, c2, 14+180, 60, 0, "Priority:          %d",task_sepcs.prio_display);
    al_draw_textf(font, c2, 14+180, 80, 0, "Deadline Miss:    %d",task_sepcs.dlmiss_display);
                
    al_draw_textf(font, c2, 14+188*2, 20, 0, "TASK KEYBOARD");
    al_draw_textf(font, c2, 14+188*2, 40, 0, "ID:            %d",task_sepcs.id_keyboard);
    al_draw_textf(font, c2, 14+188*2, 60, 0, "Priority:          %d",task_sepcs.prio_keyboard);
    al_draw_textf(font, c2, 14+188*2, 80, 0, "Deadline Miss:    %d",deadline_miss(task_sepcs.id_keyboard));

    al_draw_textf(font, c2, 14+192*3, 20, 0, "TASK WaveForms");
    al_draw_textf(font, c2, 14+192*3, 40, 0, "DLM:      %d",task_sepcs.dlmiss_sinWave);
    al_draw_textf(font, c2, 14+192*3, 60, 0, "DLM:      %d",task_sepcs.dlmiss_squWave);
    al_draw_textf(font, c2, 14+192*3, 80, 0, "DLM:      %d",task_sepcs.dlmiss_triWave);

    al_draw_textf(font, c3, 10, 240, ALLEGRO_ALIGN_LEFT,"%3.2f Hz",signal_params.freq_sin);
    al_draw_textf(font, c3, 10+297, 240, ALLEGRO_ALIGN_LEFT,"%3.2f Hz",signal_params.freq_tri);
    al_draw_textf(font, c3, 10+297*2, 240, ALLEGRO_ALIGN_LEFT,"%3.2f Hz",signal_params.freq_square);


}

/*---------------------------------------------------------------------------------------------------------*/

void Draw_Task_Bounded_Boxes(void) {
    
    /* Its a must to Set a background to render/plot over it
     * Without it the data will be accumlated on the screen (I have no Idea Why!)
     * here we plot over a filled black rectangle
     */
    al_draw_filled_rectangle(0, 0, XDISP, YDISP, c0);
    
    //BOXES for Tasks Related Information
    al_draw_rectangle(7,    10, 180, 140, c3,THICK);
    al_draw_rectangle(188,  10, 376, 140, c3,THICK);
    al_draw_rectangle(384,  10, 576, 140, c3,THICK);
    al_draw_rectangle(584,  10, 798, 140, c3,THICK);

    //BOXES to bound the waves [Sin, Tri, and Square]
    al_draw_filled_rectangle(7, 168, 300, 240, c3);
    al_draw_filled_rectangle(7+293, 168, 300+293, 240, c3);
    al_draw_filled_rectangle(7+293*2, 168, 300+293*2, 240, c3);
    
    //BOX to bound the output stream
    al_draw_rectangle(190, 300, 630, 440, c3,THICK);

}

/*---------------------------------------------------------------------------------------------------------*/

void PlotWaveforms(void){

    unsigned int i;

    //Plot the SineWave
    for (i = SINE_BLOCK_S; i < SINE_BLOCK_E; i++){
        al_put_blended_pixel(i, 200 + signal_params.sineWave_buf[i] * 20, c1);                
    }
    //Plot the TriangleWave    
    for (i=TRI_BLOCK_S ; i < TRI_BLOCK_E; i++){
        al_draw_pixel(i, 200 + signal_params.triangleWave_buf[i] * 20, c1);                
    }    
    //Plot the SquareWave      
    for (i = SQUARE_BLOCK_S; i < SQUARE_BLOCK_E; i++){
        al_draw_pixel(i, 200 + signal_params.squareWave_buf[i]* 20, c1);   
    } 
    //Plot the Output Stream [Without the SquareWave]     
    for (i = SUM_BLOCK_S; i < SUM_BLOCK_E; i++){        
        al_draw_pixel(i, 350 + signal_params.sum[i] * 50, c1);  
    } 

}

/*---------------------------------------------------------------------------------------------------------*/
void Draw_Instructions(void){

    al_draw_textf(font, c2, 1, 400, 0,"Notes: A");
    al_draw_textf(font, c3, 10, 500, NULL,"Notes:   A B C D E F G");
    al_draw_textf(font, c3, 10, 520, NULL,"KEYS:    a s d f g h j");

}
