#include "editor.h"

#include "raymath.h"

#include "raygui.h"

#include "gui.h"

#include <stdio.h>
#include <string.h>

EditorState editor_state_init(Level *lvl)
{
    EditorState state;
    state.lvl = lvl;
    strcpy(state.filename, lvl->filename);

    state.editor_camera = (Camera2D){0};
    state.editor_camera.target = (Vector2){lvl->w / 2, lvl->h / 2};
    state.editor_camera.offset = (Vector2){GetScreenWidth()/2, GetScreenHeight()/2};
    state.editor_camera.zoom = 1.0f;

    state.player_selected = false;
    state.selected_idx = -1;
    state.active_handles = 0;
    state.hovering_over_handle = false;
    state.dragging_x = false;
    state.dragging_y = false;

    state.type_dropdown = false;
    state.filename_edit = false;

    state.num_elements = 0;

    return state;
}

void entity_inspector_flag(Entity *e, Rectangle rect, enum Flags flag, const char *name)
{
    bool flag_bool = (e->flags & flag);
    GuiCheckBox(rect, name, &flag_bool);
    e->flags &= ~flag;
    e->flags |= flag_bool ? flag : 0;
}

bool MouseOverGUI(Rectangle *elements, int count)
{
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < count; i++)
        if (CheckCollisionPointRec(mouse, elements[i]))
            return true;
    return false;
}

void editor_state_update_and_draw(EditorState *state)
{
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE))
    {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/state->editor_camera.zoom);
        state->editor_camera.target = Vector2Add(state->editor_camera.target, delta);
    }

    if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT))
    {
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state->editor_camera);
        Entity *e = entity_system_add(&state->lvl->entity_system_initial, init_rect(mouseWorldPos, (Vector2){GRID_SIZE, GRID_SIZE}));

        if(!e)
            printf("ERROR: MAx entity limit reached!\n");
    }

    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), state->editor_camera);

        // Set the offset to where the mouse is
        state->editor_camera.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        state->editor_camera.target = mouseWorldPos;

        float scale = 0.2f*wheel;

        // Zoom increment
        // Uses log scaling to provide consistent zoom speed
        state->editor_camera.zoom = Clamp(expf(logf(state->editor_camera.zoom)+scale), 0.125f, 64.0f);
    }

    if(state->selected_idx == -1 && !state->player_selected)
        state->hovering_over_handle = false;
    
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !MouseOverGUI(state->elements, state->num_elements) && !state->hovering_over_handle)
    {
        int prev_idx = state->selected_idx;
        state->selected_idx = -1;
        state->player_selected = true;

        Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), state->editor_camera);
        for(int i = 0; i < state->lvl->entity_system_initial.num_elements; i++)
        {
            Entity e = state->lvl->entity_system_initial.entities[i];
            if(CheckCollisionPointRec(mouseWorld, e.collision) && prev_idx != i)
            {
                state->selected_idx = i;
                break;
            }
        }

        state->player_selected = CheckCollisionPointCircle(mouseWorld, state->lvl->initial_player.pos, PLAYER_RADIUS);
    }

    if(IsKeyPressed(KEY_ONE))
        state->active_handles = 0;
    else if(IsKeyPressed(KEY_TWO))
        state->active_handles = 1;
    else if(IsKeyPressed(KEY_THREE))
        state->active_handles = 2;

    if(state->selected_idx != -1)
    {
        if(IsKeyPressed(KEY_RIGHT))
            state->selected_idx++;
        else if(IsKeyPressed(KEY_LEFT))
            state->selected_idx--;

        if(state->selected_idx == -1)
            state->selected_idx += state->lvl->entity_system_initial.num_elements;
        else if(state->selected_idx == state->lvl->entity_system_initial.num_elements)
            state->selected_idx = 0;
    }
        
    state->num_elements = 0;


    // DRAWING

    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(state->editor_camera);

    // Grid lines
    for (int x = 0; x <= state->lvl->w; x += GRID_SIZE)
        DrawLine(x, 0, x, state->lvl->h, Fade(LIGHTGRAY, 0.4f));

    for (int y = 0; y <= state->lvl->h; y += GRID_SIZE)
        DrawLine(0, y, state->lvl->w, y, Fade(LIGHTGRAY, 0.4f));


    for(int i = 0; i < state->lvl->entity_system_initial.num_elements; i++)
    {
        draw_entity(state->lvl->entity_system_initial.entities[i], i == state->selected_idx);
    }
    
    draw_player(state->lvl->initial_player);
    if(state->player_selected)
    {
        DrawCircleLinesV(state->lvl->initial_player.pos, PLAYER_RADIUS, get_selection_color());
    }
    DrawRectangleLines(0, 0, state->lvl->w, state->lvl->h, RED);

    EndMode2D();

    // Editor GUI stuff

    DrawText("Editor Mode", GetScreenWidth() / 2 - (11 * (50/4)), GetScreenHeight() - 60, 50, BLACK);

    // Inspector

    // Draw Inspector
    Rectangle inspector = {GetScreenWidth() - 400, 0, 400, GetScreenHeight()};
    state->elements[state->num_elements++] = inspector;
    GuiPanel(inspector, "Inspector");

    // What are we inspecting...?
    if(state->player_selected) // Player
    {
        if(state->active_handles == 0)
            gui_editor_handles(state, state->lvl->initial_player.pos, &state->lvl->initial_player.pos, (Vector2){1, 1});
        gui_editor_inspector_player(state, inspector);
    }
    else if(state->selected_idx != -1) // Entity
    {
        Entity *se = &state->lvl->entity_system_initial.entities[state->selected_idx];
        
        // Handles

        if(state->active_handles == 0)
            gui_editor_handles(state, se->pos, &se->pos, (Vector2){1, 1});
        else if(state->active_handles == 1)
            gui_editor_handles(state, se->pos, &se->size, (Vector2){2, -2});

        gui_editor_inspector_entity(state, inspector);

        update_collision(se);
        put_player_in_attached_position(&state->lvl->entity_system_initial, &state->lvl->initial_player);
    }
    else // Level
    {
        gui_editor_inspector_level(state, inspector);
    }

    Rectangle filename_box = (Rectangle){430, 0, 500, 50};
    state->elements[state->num_elements++] = filename_box;
    if(GuiTextBox(filename_box, state->filename, 128, state->filename_edit))
        state->filename_edit = !state->filename_edit;

    Rectangle save = (Rectangle){10, 0, 200, 50};
    state->elements[state->num_elements++] = save;
    Rectangle load = (Rectangle){220, 0, 200, 50};
    state->elements[state->num_elements++] = load;

    if(GuiButton(save, "Save"))
        save_level(state->lvl, state->filename);
    if(GuiButton(load, "Load"))
        load_level(state->lvl, state->filename);
    
    Rectangle pos_box = (Rectangle){20, 70, 40, 40};
    Rectangle size_box = (Rectangle){20, 70, 40, 120};
    state->elements[state->num_elements++] = size_box;
    GuiToggleGroup(pos_box, "#68#\n#69#\n#21#", &state->active_handles);

    EndDrawing();
}

void editor_draw_inspector(EditorState *state)
{
    if(state->player_selected) // Player
    {
    }
    else if(state->selected_idx != -1) // Entity
    {
        // 
    }
    else // Level
    {
        // Width, height
    }
}
