#include "gui.h"
#include "editor.h"

#include "raymath.h"

#include <stdio.h>
#include <limits.h>

Color get_selection_color()
{
    float t = (sinf(GetTime() * 10.0f) + 1.0f) * 0.5f;

    Color color = {
        (unsigned char)(RED.r * t + WHITE.r * (1 - t)),
        (unsigned char)(RED.g * t + WHITE.g * (1 - t)),
        (unsigned char)(RED.b * t + WHITE.b * (1 - t)),
        255
    };
    return color;
}


void gui_editor_inspector_flags(Rectangle rect, char *flag_names[], int *flags, int num_flags)
{
    rect.width = rect.height;

    int flag = 1;
    for(int i = 0; i < num_flags; i++)
    {
        bool flag_bool = (*flags) & flag;
        GuiCheckBox(rect, flag_names[i], &flag_bool);
        rect.y += rect.height + GUI_PADDING;

        *flags = (*flags) & (~flag);
        *flags = (*flags) | (flag_bool ? flag : 0);

        flag <<= 1;
    }
}

void gui_editor_handles(EditorState *state, Vector2 pos, Vector2 *attribute, Vector2 scale)
{
    int handle_size = HANDLE_SIZE / state->editor_camera.zoom;
    if(handle_size < HANDLE_MIN_SIZE) handle_size = HANDLE_MIN_SIZE;

    int handle_length = handle_size * HANDLE_LENGTH_MULTIPLIER;

    Rectangle handlex = (Rectangle){
        pos.x + handle_length - handle_size/2,
        pos.y - handle_size/2,
        handle_size,
        handle_size
    };

    Rectangle handley = (Rectangle){
        pos.x - handle_size/2,
        pos.y - handle_length - handle_size/2,
        handle_size,
        handle_size
    };
    Rectangle handlexy = (Rectangle){
        pos.x - handle_size/2,
        pos.y - handle_size/2,
        handle_size,
        handle_size
    };

    // Draw Handles
    BeginMode2D(state->editor_camera);
    DrawLineEx((Vector2){pos.x, pos.y}, (Vector2){pos.x + handle_length, pos.y}, handle_size/3, WHITE);
    DrawLineEx((Vector2){pos.x, pos.y}, (Vector2){pos.x, pos.y - handle_length}, handle_size/3, WHITE);
    DrawRectangleRec(handlexy, LIGHTGRAY);
    DrawRectangleRec(handlex, RED);
    DrawRectangleRec(handley, BLUE);
    EndMode2D();

    Vector2 mouseWorld = GetScreenToWorld2D(GetMousePosition(), state->editor_camera);

    static Vector2 mouse_start;
    static Vector2 attribute_start;

    state->hovering_over_handle = true;
    if(!CheckCollisionPointRec(mouseWorld, handlex)
            && !CheckCollisionPointRec(mouseWorld, handley)
            && !CheckCollisionPointRec(mouseWorld, handlexy))
        state->hovering_over_handle = false;

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && state->hovering_over_handle)
    {
        mouse_start = mouseWorld;
        attribute_start = *attribute;

        if (CheckCollisionPointRec(mouseWorld, handlex))
            state->dragging_x = true;
        else if (CheckCollisionPointRec(mouseWorld, handley))
            state->dragging_y = true;
        else
        {
            state->dragging_x = true;
            state->dragging_y = true;
        }
    }

    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
    {
        state->dragging_x = false;
        state->dragging_y = false;
    }

    if(state->dragging_x)
    {
        attribute->x = attribute_start.x + (mouseWorld.x - mouse_start.x) * scale.x;
        if(IsKeyDown(KEY_LEFT_CONTROL))
            attribute->x = roundf(attribute->x / GRID_SIZE) * GRID_SIZE;
    }

    if(state->dragging_y)
    {
        attribute->y = attribute_start.y + (mouseWorld.y - mouse_start.y) * scale.y;
        if(IsKeyDown(KEY_LEFT_CONTROL))
            attribute->y = roundf(attribute->y / GRID_SIZE) * GRID_SIZE;
    }
}

void gui_editor_inspector(EditorState *state, Rectangle inspector)
{
}

void gui_editor_inspector_player(EditorState *state, Rectangle inspector)
{
    Rectangle current_element = (Rectangle){inspector.x + GUI_PADDING, inspector.y + GUI_PADDING+20, inspector.width - (GUI_PADDING*2), 40};

    // Position
    GuiLabel(current_element, "Position");
    current_element.y += current_element.height + GUI_PADDING;

    state->posx = state->lvl->initial_player.pos.x;
    state->posy = state->lvl->initial_player.pos.y;

    Rectangle attrib = current_element;
    attrib.width /= 2;
    attrib.width -= GUI_PADDING;

    if(GuiSpinner(attrib, "x", &state->posx, 0, state->lvl->w, state->posx_edit))
        state->posx_edit = !state->posx_edit;
    
    attrib.x += 2 * GUI_PADDING + attrib.width;
    if(GuiSpinner(attrib, "y", &state->posy, 0, state->lvl->h, state->posy_edit))
        state->posy_edit = !state->posy_edit;

    state->lvl->initial_player.pos.x = state->posx;
    state->lvl->initial_player.pos.y = state->posy;

    current_element.y += current_element.height + GUI_PADDING;

    // Attached to id
    static bool edit_attached_to_id = false;
    GuiLabel(current_element, "Attached to ID");
    current_element.y += current_element.height + GUI_PADDING;
    if(GuiSpinner(current_element, "", &state->lvl->initial_player.attached_id, -1, MAX_ENTITIES, edit_attached_to_id))
        edit_attached_to_id = !edit_attached_to_id;
    current_element.y += current_element.height + GUI_PADDING;

    // Attached to dir
    static bool edit_attached_to_dir = false;
    GuiLabel(current_element, "Attached to Direction");
    current_element.y += current_element.height + GUI_PADDING;
    if(GuiSpinner(current_element, "", (int*)&state->lvl->initial_player.attached_dir, 0, 3, edit_attached_to_dir))
        edit_attached_to_dir = !edit_attached_to_dir;
    
    put_player_in_attached_position(&state->lvl->entity_system_initial, &state->lvl->initial_player);
}

void gui_editor_inspector_entity(EditorState *state, Rectangle inspector)
{
    Entity *se = &state->lvl->entity_system_initial.entities[state->selected_idx];

    Rectangle current_element = (Rectangle){inspector.x + GUI_PADDING, inspector.y + GUI_PADDING+20, inspector.width - (GUI_PADDING*2), 40};

    // Entity ID
    char buf[20];
    sprintf(buf, "ID: %d", se->id);
    GuiLabel(current_element, buf);
    current_element.y += current_element.height + GUI_PADDING;

    // Entity Type
    GuiLabel(current_element, "Entity Type");
    current_element.y += current_element.height + GUI_PADDING;

    // Defer drawing dropdown box
    int dropdown_y = current_element.y;
    current_element.y += current_element.height + GUI_PADDING;

    int num_flags = 5;
    char *flag_names[] = {
        "Dynamic",
        "Socket Up",
        "Socket Down",
        "Socket Left",
        "Socket Right",
    };
    gui_editor_inspector_flags(current_element, flag_names, &se->flags, num_flags);
    current_element.y += (current_element.height + GUI_PADDING) * num_flags;

    // Position
    GuiLabel(current_element, "Position");
    current_element.y += current_element.height + GUI_PADDING;

    state->posx = se->pos.x;
    state->posy = se->pos.y;

    Rectangle attrib = current_element;
    attrib.width /= 2;
    attrib.width -= GUI_PADDING;

    if(GuiSpinner(attrib, "x", &state->posx, 0, state->lvl->w, state->posx_edit))
        state->posx_edit = !state->posx_edit;
    
    attrib.x += 2 * GUI_PADDING + attrib.width;
    if(GuiSpinner(attrib, "y", &state->posy, 0, state->lvl->h, state->posy_edit))
        state->posy_edit = !state->posy_edit;

    se->pos.x = state->posx;
    se->pos.y = state->posy;

    current_element.y += current_element.height + GUI_PADDING;

    // Size
    GuiLabel(current_element, "Size");
    current_element.y += current_element.height + GUI_PADDING;

    attrib = current_element;
    attrib.width /= 2;
    attrib.width -= GUI_PADDING;

    state->sizex = se->size.x;
    state->sizey = se->size.y;

    if(GuiSpinner(attrib, "w", &state->sizex, 0, state->lvl->w, state->sizex_edit))
        state->sizex_edit = !state->sizex_edit;

    attrib.x += 2 * GUI_PADDING + attrib.width;
    if(GuiSpinner(attrib, "h", &state->sizey, 0, state->lvl->h, state->sizey_edit))
        state->sizey_edit = !state->sizey_edit;

    current_element.y += current_element.height + GUI_PADDING;

    se->size.x = state->sizex;
    se->size.y = state->sizey;

    current_element.y = dropdown_y;

    // Deferred Dropdown (so it shows over other UI elements)
    if(GuiDropdownBox(current_element, "Socket;Rectangle;Spike", (int*)&se->type, state->type_dropdown))
        state->type_dropdown = !state->type_dropdown;
    
    // Delete
    current_element.y = inspector.y + inspector.height - GUI_PADDING - current_element.height;
    if(GuiButton(current_element, "Destroy!") || IsKeyPressed(KEY_DELETE))
    { 
       entity_system_remove_by_idx(&state->lvl->entity_system_initial, state->selected_idx);
       state->selected_idx = -1;
    }
}

void gui_editor_inspector_level(EditorState *state, Rectangle inspector)
{
    Rectangle current_element = (Rectangle){inspector.x + GUI_PADDING, inspector.y + GUI_PADDING+20, inspector.width - (GUI_PADDING*2), 40};

    // Dimensions
    GuiLabel(current_element, "Size");
    current_element.y += current_element.height + GUI_PADDING;

    state->sizex = state->lvl->w;
    state->sizey = state->lvl->h;

    Rectangle attrib = current_element;
    attrib.width /= 2;
    attrib.width -= GUI_PADDING;

    if(GuiSpinner(attrib, "w", &state->sizex, 0, INT_MAX, state->sizex_edit))
        state->sizex_edit = !state->sizex_edit;
    
    attrib.x += 2 * GUI_PADDING + attrib.width;
    if(GuiSpinner(attrib, "h", &state->sizey, 0, INT_MAX, state->sizey_edit))
        state->sizey_edit = !state->sizey_edit;

    state->lvl->w = state->sizex;
    state->lvl->h = state->sizey;

    current_element.y += current_element.height + GUI_PADDING;
}
