#include <lem_in.h>

t_room *create_room(const char *name, int x, int y)
{
    t_room *room = (t_room *)malloc(sizeof(t_room));
    if (!room) return NULL;
    room->name = strdup(name);
    room->x = x;
    room->y = y;
    room->ant_id = 0;
    room->is_start = 0;
    room->is_end = 0;
    room->visited = 0;
    room->distance = -1;
    room->links = NULL;
    room->num_links = 0;
    room->parent = NULL;
    room->parent_link = NULL;
    return room;
}

void add_link(t_room *room1, t_room *room2, int capacity)
{
    t_link *link1_to_2 = (t_link *)malloc(sizeof(t_link));
    if (!link1_to_2)
        return;
    link1_to_2->target_room = room2;
    link1_to_2->capacity = capacity;
    link1_to_2->flow = 0;

    t_link *link2_to_1 = (t_link *)malloc(sizeof(t_link));
    if (!link2_to_1)
    {
        free(link1_to_2);
        return;
    }
    link2_to_1->target_room = room1;
    link2_to_1->capacity = capacity;
    link2_to_1->flow = 0;

    link1_to_2->reverse_link = link2_to_1;
    link2_to_1->reverse_link = link1_to_2;

    room1->links = (t_link **)realloc(room1->links, sizeof(t_link *) * (room1->num_links + 1));
    if (!room1->links)
    {
        free(link1_to_2);
        free(link2_to_1);
        return;
    }
    room1->links[room1->num_links++] = link1_to_2;

    room2->links = (t_link **)realloc(room2->links, sizeof(t_link *) * (room2->num_links + 1));
    if (!room2->links)
    {
        free(link1_to_2);
        free(link2_to_1);
        return;
    }
    room2->links[room2->num_links++] = link2_to_1;
}


t_antfarm *hardcode_init_antfarm_example1()
{
    t_antfarm *farm = (t_antfarm *)malloc(sizeof(t_antfarm));
    if (!farm)
        return NULL;

    farm->num_ants = 3;
    farm->num_rooms = 4;
    farm->rooms = (t_room **)malloc(sizeof(t_room *) * farm->num_rooms);
    if (!farm->rooms)
    {
        free(farm);
        return NULL;
    }

    t_room *s_room = create_room("0", 1, 0);
    t_room *e_room = create_room("1", 5, 0);
    t_room *room2 = create_room("2", 9, 0);
    t_room *room3 = create_room("3", 13, 0);

    if (!s_room || !e_room || !room2 || !room3)
    {
        if (s_room)
        { free(s_room->name); free(s_room); }
        if (e_room)
        { free(e_room->name); free(e_room); }
        if (room2)
        { free(room2->name); free(room2); }
        if (room3)
        { free(room3->name); free(room3); }
        free(farm->rooms);
        free(farm);
        return NULL;
    }

    farm->rooms[0] = s_room;
    farm->rooms[1] = e_room;
    farm->rooms[2] = room2;
    farm->rooms[3] = room3;

    farm->start_room = s_room;
    farm->end_room = e_room;
    s_room->is_start = 1;
    e_room->is_end = 1;

    add_link(s_room, room2, 1);
    add_link(room2, room3, 1);
    add_link(room3, e_room, 1);

    farm->ants = (t_ant *)malloc(sizeof(t_ant) * farm->num_ants);
    if (!farm->ants)
    {
        free_antfarm(farm);
        return NULL;
    }
    for (int i = 0; i < farm->num_ants; i++)
    {
        farm->ants[i].id = i + 1;
        farm->ants[i].current_room = farm->start_room;
        farm->ants[i].current_path_step = 0;
        farm->ants[i].finished = false;
        farm->ants[i].path_id = -1;
    }

    return farm;
}

t_antfarm *hardcode_init_antfarm_variable_length_paths()
{
    t_antfarm *farm = (t_antfarm *)malloc(sizeof(t_antfarm));
    if (!farm) 
        return NULL;

    farm->num_ants = 4;
    farm->num_rooms = 17;
    farm->rooms = (t_room **)malloc(sizeof(t_room *) * farm->num_rooms);
    if (!farm->rooms)
    {
        free(farm);
        return NULL;
    }

    t_room *s_room = create_room("S", 0, 0);
    t_room *e_room = create_room("E", 10, 0);
    if (!s_room || !e_room)
    {
        free(farm->rooms);
        free(farm);
        return NULL;
    }

    farm->rooms[0] = s_room; farm->rooms[1] = e_room;
    farm->start_room = s_room; farm->end_room = e_room;
    s_room->is_start = 1; e_room->is_end = 1;

    t_room *p1_rooms[3];
    p1_rooms[0] = create_room("P1_1", 2, 0); p1_rooms[1] = create_room("P1_2", 4, 0); p1_rooms[2] = create_room("P1_3", 6, 0);
    if (!p1_rooms[0] || !p1_rooms[1] || !p1_rooms[2])
    { free(s_room->name); free(s_room); free(e_room->name); free(e_room); free(farm->rooms); free(farm); return NULL; }
    farm->rooms[2] = p1_rooms[0]; farm->rooms[3] = p1_rooms[1]; farm->rooms[4] = p1_rooms[2];

    t_room *p2_rooms[5];
    p2_rooms[0] = create_room("P2_1", 2, 2); p2_rooms[1] = create_room("P2_2", 3, 2); p2_rooms[2] = create_room("P2_3", 5, 2); p2_rooms[3] = create_room("P2_4", 7, 2); p2_rooms[4] = create_room("P2_5", 9, 2);
    if (!p2_rooms[0]||!p2_rooms[1]||!p2_rooms[2]||!p2_rooms[3]||!p2_rooms[4])
    { free(s_room->name); free(s_room); free(e_room->name); free(e_room); for(int i=0; i<3; i++)
        { free(p1_rooms[i]->name); free(p1_rooms[i]); } free(farm->rooms); free(farm); return NULL; }
    farm->rooms[5] = p2_rooms[0]; farm->rooms[6] = p2_rooms[1]; farm->rooms[7] = p2_rooms[2]; farm->rooms[8] = p2_rooms[3]; farm->rooms[9] = p2_rooms[4];

    t_room *p3_rooms[7];
    p3_rooms[0] = create_room("P3_1", 2, 4); p3_rooms[1] = create_room("P3_2", 3, 4); p3_rooms[2] = create_room("P3_3", 4, 4); p3_rooms[3] = create_room("P3_4", 6, 4); p3_rooms[4] = create_room("P3_5", 7, 4); p3_rooms[5] = create_room("P3_6", 8, 4); p3_rooms[6] = create_room("P3_7", 9, 4);
    if (!p3_rooms[0]||!p3_rooms[1]||!p3_rooms[2]||!p3_rooms[3]||!p3_rooms[4]||!p3_rooms[5]||!p3_rooms[6])
    { free(s_room->name); free(s_room); free(e_room->name); free(e_room); for(int i=0; i<3; i++)
        { free(p1_rooms[i]->name); free(p1_rooms[i]); } for(int i=0; i<5; i++)
    { free(p2_rooms[i]->name); free(p2_rooms[i]); } free(farm->rooms); free(farm); return NULL; }
    farm->rooms[10] = p3_rooms[0]; farm->rooms[11] = p3_rooms[1]; farm->rooms[12] = p3_rooms[2]; farm->rooms[13] = p3_rooms[3]; farm->rooms[14] = p3_rooms[4]; farm->rooms[15] = p3_rooms[5]; farm->rooms[16] = p3_rooms[6];

    add_link(s_room, p1_rooms[0], 1);
    add_link(s_room, p2_rooms[0], 1);
    add_link(s_room, p3_rooms[0], 1);

    add_link(p1_rooms[0], p1_rooms[1], 1);
    add_link(p1_rooms[1], p1_rooms[2], 1);
    add_link(p1_rooms[2], e_room, 1);

    for (int i = 0; i < 4; i++)
        add_link(p2_rooms[i], p2_rooms[i+1], 1);
    add_link(p2_rooms[4], e_room, 1);

    for (int i = 0; i < 6; i++)
        add_link(p3_rooms[i], p3_rooms[i+1], 1);
    add_link(p3_rooms[6], e_room, 1);

    farm->ants = (t_ant *)malloc(sizeof(t_ant) * farm->num_ants);
    if (!farm->ants)
    {
        free_antfarm(farm);
        return NULL;
    }

    for (int i = 0; i < farm->num_ants; i++)
    {
        farm->ants[i].id = i + 1;
        farm->ants[i].current_room = farm->start_room;
        farm->ants[i].path_id = -1;
    }

    return farm;
}

t_antfarm *hardcode_init_antfarm_example2_1()
{
    t_antfarm *farm = (t_antfarm *)malloc(sizeof(t_antfarm));
    if (!farm) return NULL;

    farm->num_ants = 2;
    farm->num_rooms = 5;
    farm->rooms = (t_room **)malloc(sizeof(t_room *) * farm->num_rooms);
    if (!farm->rooms)
    {
        free(farm);
        return NULL;
    }

    t_room *s_room = create_room("0", 2, 0);
    t_room *e_room = create_room("4", 2, 6);
    t_room *room1 = create_room("1", 0, 2);
    t_room *room2 = create_room("2", 4, 2);
    t_room *room3 = create_room("3", 4, 4);

    if (!s_room || !e_room || !room1 || !room2 || !room3)
    {
        if (s_room)
        { free(s_room->name); free(s_room); }
        if (e_room)
        { free(e_room->name); free(e_room); }
        if (room1)
        { free(room1->name); free(room1); }
        if (room2)
        { free(room2->name); free(room2); }
        if (room3)
        { free(room3->name); free(room3); }
        free(farm->rooms);
        free(farm);
        return NULL;
    }

    farm->rooms[0] = s_room;
    farm->rooms[1] = room1;
    farm->rooms[2] = room2;
    farm->rooms[3] = room3;
    farm->rooms[4] = e_room;

    farm->start_room = s_room;
    farm->end_room = e_room;
    s_room->is_start = 1;
    e_room->is_end = 1;

    add_link(s_room, room1, 1);
    add_link(s_room, room2, 1);
    add_link(room2, room3, 1);
    add_link(room3, e_room, 1);
    add_link(e_room, room1, 1);

    farm->ants = (t_ant *)malloc(sizeof(t_ant) * farm->num_ants);
    if (!farm->ants)
    {
        free_antfarm(farm);
        return NULL;
    }
    for (int i = 0; i < farm->num_ants; i++)
    {
        farm->ants[i].id = i + 1;
        farm->ants[i].current_room = farm->start_room;
        farm->ants[i].current_path_step = 0;
        farm->ants[i].finished = false;
        farm->ants[i].path_id = -1;
    }

    return farm;
}

t_antfarm *hardcode_init_antfarm_example2_2()
{
    t_antfarm *farm = (t_antfarm *)malloc(sizeof(t_antfarm));
    if (!farm)
        return NULL;

    farm->num_ants = 3;
    farm->num_rooms = 5;
    farm->rooms = (t_room **)malloc(sizeof(t_room *) * farm->num_rooms);
    if (!farm->rooms)
    {
        free(farm);
        return NULL;
    }

    t_room *s_room = create_room("0", 2, 0);
    t_room *e_room = create_room("4", 2, 6);
    t_room *room1 = create_room("1", 0, 2);
    t_room *room2 = create_room("2", 4, 2);
    t_room *room3 = create_room("3", 4, 4);

    if (!s_room || !e_room || !room1 || !room2 || !room3)
    {
        if (s_room)
        { free(s_room->name); free(s_room); }
        if (e_room)
        { free(e_room->name); free(e_room); }
        if (room1)
        { free(room1->name); free(room1); }
        if (room2)
        { free(room2->name); free(room2); }
        if (room3)
        { free(room3->name); free(room3); }
        free(farm->rooms);
        free(farm);
        return NULL;
    }

    farm->rooms[0] = s_room;
    farm->rooms[1] = room1;
    farm->rooms[2] = room2;
    farm->rooms[3] = room3;
    farm->rooms[4] = e_room;

    farm->start_room = s_room;
    farm->end_room = e_room;
    s_room->is_start = 1;
    e_room->is_end = 1;

    add_link(s_room, room1, 1);
    add_link(s_room, room2, 1);
    add_link(room2, room3, 1);
    add_link(room3, e_room, 1);
    add_link(e_room, room1, 1);

    farm->ants = (t_ant *)malloc(sizeof(t_ant) * farm->num_ants);
    if (!farm->ants)
    {
        free_antfarm(farm);
        return NULL;
    }
    for (int i = 0; i < farm->num_ants; i++)
    {
        farm->ants[i].id = i + 1;
        farm->ants[i].current_room = farm->start_room;
        farm->ants[i].current_path_step = 0;
        farm->ants[i].finished = false;
        farm->ants[i].path_id = -1;
    }

    return farm;
}

t_antfarm *hardcode_init_antfarm_example3()
{
    t_antfarm *farm = (t_antfarm *)malloc(sizeof(t_antfarm));
    if (!farm)
        return NULL;

    farm->num_ants = 4;
    farm->num_rooms = 8;
    farm->rooms = (t_room **)malloc(sizeof(t_room *) * farm->num_rooms);
    if (!farm->rooms)
    {
        free(farm);
        return NULL;
    }

    t_room *s_room = create_room("start", 4, 0);
    t_room *e_room = create_room("end", 4, 6);
    t_room *room1 = create_room("1", 4, 2);
    t_room *room2 = create_room("2", 4, 4);
    t_room *room3 = create_room("3", 2, 2);
    t_room *room4 = create_room("4", 0, 4);
    t_room *room5 = create_room("5", 8, 2);
    t_room *room6 = create_room("6", 8, 4);

    if (!s_room || !e_room || !room1 || !room2 || !room3 || !room4 || !room5 || !room6)
    {
        if (s_room)
        { free(s_room->name); free(s_room); }
        if (e_room)
        { free(e_room->name); free(e_room); }
        if (room1)
        { free(room1->name); free(room1); }
        if (room2)
        { free(room2->name); free(room2); }
        if (room3)
        { free(room3->name); free(room3); }
        if (room4)
        { free(room4->name); free(room4); }
        if (room5)
        { free(room5->name); free(room5); }
        if (room6)
        { free(room6->name); free(room6); }
        free(farm->rooms);
        free(farm);
        return NULL;
    }

    farm->rooms[0] = s_room;
    farm->rooms[1] = e_room;
    farm->rooms[2] = room1;
    farm->rooms[3] = room2;
    farm->rooms[4] = room3;
    farm->rooms[5] = room4;
    farm->rooms[6] = room5;
    farm->rooms[7] = room6;

    farm->start_room = s_room;
    farm->end_room = e_room;
    s_room->is_start = 1;
    e_room->is_end = 1;

    add_link(s_room, room1, 1);
    add_link(room3, room4, 1);
    add_link(room2, room4, 1);
    add_link(room1, room5, 1);
    add_link(room6, room5, 1);
    add_link(e_room, room6, 1);
    add_link(room1, room2, 1);
    add_link(room2, e_room, 1);
    add_link(room3, s_room, 1);

    farm->ants = (t_ant *)malloc(sizeof(t_ant) * farm->num_ants);
    if (!farm->ants)
    {
        free_antfarm(farm);
        return NULL;
    }

    for (int i = 0; i < farm->num_ants; i++)
    {
        farm->ants[i].id = i + 1;
        farm->ants[i].current_room = farm->start_room;
        farm->ants[i].current_path_step = 0;
        farm->ants[i].finished = false;
        farm->ants[i].path_id = -1;
    }

    return farm;
}