#include <lem_in.h>

#define MAX_ROOMS 10 // Adjust as needed

typedef struct s_room {
    char   *name;
    int    x;
    int    y;
    int    ant_id;
    int    is_start;
    int    is_end;
    int    visited;
    int    distance;
    struct s_room **neighbors;
    int    neighbor_count;
} t_room;

typedef struct s_ant {
    int    id;
    char   *ant_name;
    t_room *current_room;
} t_ant;

typedef struct s_antfarm {
    int     number_of_ants;
    t_room  *rooms[MAX_ROOMS];
    int     room_count;
    t_ant   *ants;
    t_room  *start_room;
    t_room  *end_room;
} t_antfarm;

t_room *create_room(char *name, int x, int y, int is_start, int is_end) {
    t_room *room = (t_room *)malloc(sizeof(t_room));
    if (!room) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    room->name = strdup(name);
    room->x = x;
    room->y = y;
    room->ant_id = 0;
    room->is_start = is_start;
    room->is_end = is_end;
    room->visited = 0;
    room->distance = 0;
    room->neighbors = (struct s_room **)malloc(MAX_ROOMS * sizeof(struct s_room *)); //Initial allocation
    if (!room->neighbors) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    room->neighbor_count = 0;
    return room;
}

void add_room_to_farm(t_antfarm *farm, t_room *room) {
    farm->rooms[farm->room_count++] = room;
}

void connect_rooms(t_room *room1, t_room *room2) {
    room1->neighbors[room1->neighbor_count++] = room2;
    room2->neighbors[room2->neighbor_count++] = room1;
}

void initialize_ants(t_antfarm *farm) {
    farm->ants = (t_ant *)malloc(farm->number_of_ants * sizeof(t_ant));
    if (!farm->ants) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < farm->number_of_ants; i++) {
        farm->ants[i].id = i + 1;
        farm->ants[i].ant_name = (char *)malloc(5 * sizeof(char)); // Allocate space for "L" + id + '\0'
        if (!farm->ants[i].ant_name) {
            perror("malloc failed");
            exit(EXIT_FAILURE);
        }
        sprintf(farm->ants[i].ant_name, "L%d", i + 1);
        farm->ants[i].current_room = farm->start_room;
    }
}

void find_start_end_rooms(t_antfarm *farm) {
    for (int i = 0; i < farm->room_count; i++) {
        if (farm->rooms[i]->is_start) {
            farm->start_room = farm->rooms[i];
        }
        if (farm->rooms[i]->is_end) {
            farm->end_room = farm->rooms[i];
        }
    }
}

void hardcode_example(t_antfarm *farm) {
    farm->number_of_ants = 20;
    farm->room_count = 0;

    // Create rooms
    t_room *room1 = create_room("1", 23, 3, 1, 0); // start
    t_room *room2 = create_room("2", 16, 7, 0, 0);
    t_room *room3 = create_room("3", 16, 3, 0, 0);
    t_room *room4 = create_room("4", 16, 5, 0, 0);
    t_room *room5 = create_room("5", 9, 3, 0, 0);
    t_room *room6 = create_room("6", 1, 5, 0, 0);
    t_room *room7 = create_room("7", 4, 8, 0, 0);
    t_room *room0 = create_room("0", 9, 5, 0, 1); // end

    // Add rooms to the farm
    add_room_to_farm(farm, room1);
    add_room_to_farm(farm, room2);
    add_room_to_farm(farm, room3);
    add_room_to_farm(farm, room4);
    add_room_to_farm(farm, room5);
    add_room_to_farm(farm, room6);
    add_room_to_farm(farm, room7);
    add_room_to_farm(farm, room0);

    // Connect rooms
    connect_rooms(room0, room4);
    connect_rooms(room0, room6);
    connect_rooms(room1, room3);
    connect_rooms(room4, room3);
    connect_rooms(room5, room2);
    connect_rooms(room3, room5);
    connect_rooms(room4, room2);
    connect_rooms(room2, room1);
    connect_rooms(room7, room6);
    connect_rooms(room7, room2);
    connect_rooms(room7, room4);
    connect_rooms(room6, room5);

    // Find start and end rooms
    find_start_end_rooms(farm);

    // Initialize ants
    initialize_ants(farm);
}

void print_ant_farm_data(t_antfarm *farm) {
    printf("%d\n", farm->number_of_ants);

    for (int i = 0; i < farm->room_count; i++) {
        printf("%s %d %d\n", farm->rooms[i]->name, farm->rooms[i]->x, farm->rooms[i]->y);
    }

    for (int i = 0; i < farm->room_count; i++) {
        for (int j = 0; j < farm->rooms[i]->neighbor_count; j++) {
            // Avoid printing duplicates (0-4 and 4-0)
            if (strcmp(farm->rooms[i]->name, farm->rooms[i]->neighbors[j]->name) < 0) {
                printf("%s-%s\n", farm->rooms[i]->name, farm->rooms[i]->neighbors[j]->name);
            }
        }
    }
    printf("\n");
}

void free_antfarm(t_antfarm *farm) {
    for (int i = 0; i < farm->room_count; i++) {
        free(farm->rooms[i]->name);
        free(farm->rooms[i]->neighbors);
        free(farm->rooms[i]);
    }
    for (int i = 0; i < farm->number_of_ants; i++) {
        free(farm->ants[i].ant_name);
    }
    free(farm->ants);
}

int main() {
    t_antfarm farm;
    farm.room_count = 0; // Initialize room_count

    hardcode_example(&farm);
    print_ant_farm_data(&farm);

    // The rest of your algorithm and output will go here...

    free_antfarm(&farm);

    return 0;
}