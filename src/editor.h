#ifndef _EDITOR_H
#define _EDITOR_H

#include "level.h"

#define GRID_SIZE 40

typedef struct EditorState {
    char filename[128];
    Level *lvl;
    Camera2D editor_camera;

    // Selected
    bool player_selected;
    int selected_idx;
    int active_handles;
    bool hovering_over_handle;

    bool dragging_x;
    bool dragging_y;

    // Inspector
    int posx, posy;
    int sizex, sizey;
    int w, h;

    bool posx_edit, posy_edit;
    bool sizex_edit, sizey_edit;
    bool w_edit, h_edit;

    bool type_dropdown;
    bool filename_edit;

    int num_elements;
    Rectangle elements[100];
} EditorState;

EditorState editor_state_init(Level *lvl);

void editor_state_update_and_draw(EditorState *state);

#endif
