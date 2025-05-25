#include <lem_in.h>

void free_antfarm(t_antfarm *farm)
{
    if (!farm)
        return;

    for (int i = 0; i < farm->num_rooms; i++)
    {
        if (farm->rooms[i])
        {
            for (int j = 0; j < farm->rooms[i]->num_links; j++)
                free(farm->rooms[i]->links[j]);
            free(farm->rooms[i]->links);
            free(farm->rooms[i]->name);
            free(farm->rooms[i]);
        }
    }

    free(farm->rooms);
    free(farm->ants);
    free(farm);
}

// Resets the visited, distance, parent and parent_link variables of each room. Usefull at the start of bfs.
void reset_room_states_for_bfs(t_antfarm *farm)
{
    for (int i = 0; i < farm->num_rooms; i++)
    {
        farm->rooms[i]->visited = 0;
        farm->rooms[i]->distance = -1;
        farm->rooms[i]->parent = NULL;
        farm->rooms[i]->parent_link = NULL;
    }
}

// Extracts all the paths that Edmonds karp and BFS found and saves them in an array of t_lem_path*.
t_lem_path **extract_all_paths(t_antfarm *farm, int *num_extracted_paths)
{
    *num_extracted_paths = 0;
    t_lem_path **extracted_paths = NULL;

    int paths_found_this_run = 0;

    while (paths_found_this_run < farm->max_flow)
    {
        reset_room_states_for_bfs(farm);

        t_room **queue = (t_room **)malloc(sizeof(t_room *) * farm->num_rooms);
        if (!queue)
        {
            if (extracted_paths)
            {
                for (int i = 0; i < *num_extracted_paths; i++)
                {
                    free(extracted_paths[i]->rooms);
                    free(extracted_paths[i]);
                }
                free(extracted_paths);
            }
            return NULL;
        }
        int head = 0;
        int tail = 0;

        farm->start_room->visited = 1;
        farm->start_room->distance = 0;
        queue[tail++] = farm->start_room;

        t_room *current = NULL;
        bool path_found_in_bfs = false;

        while (head < tail)
        {
            current = queue[head++];

            if (current == farm->end_room)
            {
                path_found_in_bfs = true;
                break;
            }

            for (int i = 0; i < current->num_links; i++)
            {
                t_link *link = current->links[i];
                t_room *neighbor = link->target_room;

                if (neighbor->visited == 0 && link->flow > 0)
                {
                    neighbor->visited = 1;
                    neighbor->distance = current->distance + 1;
                    neighbor->parent = current;
                    neighbor->parent_link = link;
                    queue[tail++] = neighbor;
                }
            }
        }
        free(queue);

        if (path_found_in_bfs)
        {
            paths_found_this_run++;

            t_lem_path *new_path = (t_lem_path *)malloc(sizeof(t_lem_path));
            if (!new_path)
            {
                if (extracted_paths)
                {
                    for (int i = 0; i < *num_extracted_paths; i++)
                    {
                        free(extracted_paths[i]->rooms);
                        free(extracted_paths[i]);
                    }
                    free(extracted_paths);
                }
                return NULL;
            }
            new_path->rooms = NULL;
            new_path->length = 0;
            new_path->ants_assigned = 0;
            new_path->next_finish_turn = 0;

            current = farm->end_room;
            while (current != NULL)
            {
                new_path->length++;
                current = current->parent;
            }

            new_path->rooms = (t_room **)malloc(sizeof(t_room *) * new_path->length);
            if (!new_path->rooms)
            {
                free(new_path);
                if (extracted_paths)
                {
                    for (int i = 0; i < *num_extracted_paths; i++)
                    {
                        free(extracted_paths[i]->rooms);
                        free(extracted_paths[i]);
                    }
                    free(extracted_paths);
                }
                return NULL;
            }

            int path_room_idx = new_path->length - 1;
            current = farm->end_room;
            while (current != NULL)
            {
                new_path->rooms[path_room_idx--] = current;

                if (current->parent_link)
                {
                    current->parent_link->flow--;
                    current->parent_link->reverse_link->flow++;
                }
                current = current->parent;
            }

            new_path->next_finish_turn = new_path->length;

            extracted_paths = (t_lem_path **)realloc(extracted_paths, sizeof(t_lem_path *) * (*num_extracted_paths + 1));
            if (!extracted_paths)
            {
                free(new_path->rooms);
                free(new_path);
                if (extracted_paths)
                {
                    for (int i = 0; i < *num_extracted_paths; i++)
                    {
                        free(extracted_paths[i]->rooms);
                        free(extracted_paths[i]);
                    }
                    free(extracted_paths);
                }
                return NULL;
            }
            extracted_paths[*num_extracted_paths] = new_path;
            (*num_extracted_paths)++;
        }
        else
        {
            fprintf(stderr, "Warning: Could not extract all %d expected paths. Only found %d.\n", farm->max_flow, paths_found_this_run);
            break;
        }
    }
    return extracted_paths;
}

// Assigns every ant to a path depending on the weight of that path.
void    assign_ant_to_path(t_antfarm *farm, t_lem_path **paths, int num_paths)
{
    for (int i = 0; i < farm->num_ants; i++)
    {
        farm->ants[i].current_room = farm->start_room;
        farm->ants[i].current_path_step = 0;
        farm->ants[i].finished = false;
        farm->ants[i].path_id = -1;
    }
    
    int ants_to_assign = farm->num_ants;
    for (int ant_idx = 0; ant_idx < ants_to_assign; ant_idx++)
    {
        int best_path_idx = -1;
        int min_finish_turn = -1;
        
        for (int i = 0; i < num_paths; i++)
        {
            if (best_path_idx == -1 || paths[i]->next_finish_turn < min_finish_turn)
            {
                min_finish_turn = paths[i]->next_finish_turn;
                best_path_idx = i;
            }
        }
        
        farm->ants[ant_idx].path_id = best_path_idx;
        paths[best_path_idx]->ants_assigned++;
        paths[best_path_idx]->next_finish_turn++;
    }
}

// Outputs the movement of the ants.
void    simulate(t_antfarm *farm, t_lem_path **paths, int num_paths)
{
    (void) num_paths;
    bool all_ants_finished = false;
    int  ants_at_end_count = 0;
    int  turn_number = 0;
    
    while (!all_ants_finished)
    {
        turn_number++;
        char    *current_turn_moves[farm->num_ants + 1];
        int     moves_in_turn = 0;
        
        t_room  *occupied_in_turn[farm->num_rooms];
        int     num_occupied_in_turn = 0;
        
        for (int i = 0; i < farm->num_ants; i++)
        {
            t_ant *ant = &farm->ants[i];
            
            if (ant->finished)
            continue;
            
            t_lem_path *ant_path = paths[ant->path_id];
            
            if (ant->current_path_step + 1 < ant_path->length)
            {
                t_room *next_room = ant_path->rooms[ant->current_path_step + 1];
                
                bool can_move = true;
                if (next_room != farm->end_room)
                {
                    for (int k = 0; k < num_occupied_in_turn; k++)
                    {
                        if (occupied_in_turn[k] == next_room)
                        {
                            can_move = false;
                            break;
                        }
                    }
                }
                
                if (can_move)
                {
                    if (ant->current_room != farm->start_room && ant->current_room != farm->end_room)
                    ant->current_room->ant_id = 0;
                    
                    ant->current_room = next_room;
                    ant->current_path_step++;
                    
                    if (next_room != farm->end_room)
                    {
                        occupied_in_turn[num_occupied_in_turn++] = next_room;
                        next_room->ant_id = ant->id;
                    }
                    
                    char *move_str = (char *)malloc(10 + snprintf(NULL, 0, "%d", ant->id) + strlen(next_room->name));
                    if (move_str)
                    {
                        sprintf(move_str, "L%d-%s", ant->id, next_room->name);
                        current_turn_moves[moves_in_turn++] = move_str;
                    }
                }
            }
            if (ant->current_room == farm->end_room && !ant->finished)
            {
                ant->finished = true;
                ants_at_end_count++;
            }
        }
        
        if (moves_in_turn > 0)
        {
            for (int i = 0; i < moves_in_turn; i++)
            {
                printf("%s%s", current_turn_moves[i], (i == moves_in_turn - 1) ? "" : " ");
                free(current_turn_moves[i]);
            }
            printf("\n");
        }
        else
        if (ants_at_end_count == farm->num_ants)
        all_ants_finished = true;
        
        if (ants_at_end_count == farm->num_ants)
        all_ants_finished = true;
    }
}

// Performs BFS to find the shortest path of the farm.
bool bfs_for_edmonds_karp(t_antfarm *farm)
{
    reset_room_states_for_bfs(farm);

    t_room **queue = (t_room **)malloc(sizeof(t_room *) * farm->num_rooms);
    if (!queue)
        return false;
    int head = 0;
    int tail = 0;

    farm->start_room->visited = 1;
    farm->start_room->distance = 0;

    queue[tail++] = farm->start_room;

    bool path_found = false;

    while (head < tail)
    {
        t_room *current = queue[head++];

        if (current == farm->end_room)
        {
            path_found = true;
            break;
        }

        for (int i = 0; i < current->num_links; i++)
        {
            t_link *current_link = current->links[i];
            t_room *neighbor = current_link->target_room;

            if (neighbor->visited == 0 && (current_link->capacity - current_link->flow) > 0)
            {
                neighbor->visited = 1;
                neighbor->distance = current->distance + 1;
                neighbor->parent = current;
                neighbor->parent_link = current_link;
                queue[tail++] = neighbor;
            }
        }
    }
    free(queue);
    return path_found;
}

// Repetedly uses BFS to find the shortests paths of the farm adn extracts the flow of each path.
void find_all_edmonds_karp(t_antfarm *farm)
{
    farm->max_flow = 0;

    while (bfs_for_edmonds_karp(farm))
    {
        int path_flow = 2147483647;

        t_room *current = farm->end_room;
        while (current != farm->start_room)
        {
            t_link *link_to_parent = current->parent_link;
            int residual_capacity = link_to_parent->capacity - link_to_parent->flow;
            if (residual_capacity < path_flow)
                path_flow = residual_capacity;
            current = current->parent;
        }

        current = farm->end_room;
        while (current != farm->start_room)
        {
            t_link *forward_link = current->parent_link;
            forward_link->flow += path_flow;
            forward_link->reverse_link->flow -= path_flow;
            current = current->parent;
        }
        farm->max_flow += path_flow;
    }
}

int main()
{
    t_antfarm *farm = NULL;
    
    farm = hardcode_init_antfarm_example3();
    
    if (farm)
    {
        int num_extracted_paths = 0;
        find_all_edmonds_karp(farm);

        t_lem_path **all_paths = extract_all_paths(farm, &num_extracted_paths);
        if (!all_paths)
        {
            fprintf(stderr, "Error: Failed to extract paths.\n");
            free_antfarm(farm);
            return 1;
        }

        assign_ant_to_path(farm, all_paths, num_extracted_paths);
        
        simulate(farm, all_paths, num_extracted_paths);

        for (int i = 0; i < num_extracted_paths; i++)
        {
            free(all_paths[i]->rooms);
            free(all_paths[i]);
        }
        free(all_paths);
        free_antfarm(farm);
    }
    else
        printf("Failed to initialize ant farm. Critical error!\n");

    return 0;
}