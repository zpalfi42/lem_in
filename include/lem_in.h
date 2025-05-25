#ifndef LEM_IN_H
#define LEM_IN_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct s_room t_room;
typedef struct s_ant t_ant;
typedef struct s_antfarm t_antfarm;
typedef struct s_link t_link;
typedef struct s_lem_path t_lem_path;

typedef struct s_link {
    t_room  *target_room;
    int     capacity;
    int     flow;
    struct  s_link  *reverse_link;
} t_link;

typedef struct s_room {
    char        *name;
    int         x;
    int         y;
    int         ant_id;
    int         is_start;
    int         is_end;
    int         visited;
    int         distance;
    t_room      *parent;
    t_link      *parent_link;
    
    t_link      **links;
    int         num_links;
} t_room;

typedef struct s_lem_path {
    t_room  **rooms;
    int     length;
    int     ants_assigned;
    int     next_finish_turn;
} t_lem_path;

typedef struct s_ant {
    int     id;
    int     path_id;
    t_room  *current_room;
    int     current_path_step;
    bool    finished;
} t_ant;

struct s_antfarm {
    int     num_ants;
    int     num_rooms;
    int     max_flow;
    t_room  **rooms;
    t_ant   *ants;
    t_room  *start_room;
    t_room  *end_room;
};

t_antfarm *hardcode_init_antfarm_example1( void );
t_antfarm *hardcode_init_antfarm_variable_length_paths( void );
t_antfarm *hardcode_init_antfarm_example2_1( void );
t_antfarm *hardcode_init_antfarm_example2_2( void );
t_antfarm *hardcode_init_antfarm_example3( void );

void free_antfarm(t_antfarm *farm);

#endif