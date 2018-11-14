#include <stdio.h> 
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "core/data_types.h"
#include "core/idx_manip.h"
#include "core/util.h"

#define INVALID_MENU_OPTION -1

typedef enum {
    SELECT = 1,
    INSERT = 2,
    UPDATE = 3,
    DELETE = 4
} MainMenuOpts;

typedef enum {
    SELECT_ALL_ALBUMS = 1,
    SELECT_ALL_TRACKS = 2,
    SELECT_ALBUM_BY_ID = 3,
    SELECT_TRACK_BY_ID = 4,
    SELECT_ALBUM_TRACKS = 5
} SelectOpts;

typedef enum {
    ALBUM_OPTION = 1,
    TRACK_OPTION = 2
} CommonOpts;


void display_album(Album album, bool with_header) {
    if (with_header) {
        printf("ID | ALBUM_NAME | RELEASE_DATE\n");
    }
    printf("%d:  %s  %d\n", album.id, album.name, album.release_date);
}

void display_track(Track track, bool with_header) {
    if (with_header) {
        printf("ID | TRACK_NAME | ARTIST_NAME | LENGTH_IN_SECONDS\n");
    }
    printf(
        "%d:  %s  %s  %d\n", 
        track.id, track.name, track.artist_name, track.length_in_seconds
    );
}

void display_all_albums() {
    uint64_t records_amount = get_albums_records_amount();
    for (uint64_t i=0; i<records_amount; i++) {
        Album selected = get_album(i);
        display_album(selected, !i);
    }
}

void display_all_tracks() {
    uint64_t records_amount = get_tracks_records_amount();
    for (uint64_t i=0; i<records_amount; i++) {
        Track selected = get_track(i);
        display_track(selected, !i);
    }
}

void display_album_tracks(uint64_t album_id) {
    size_t track_ids_size = get_album_idx_size(album_id);
    uint64_t tracks_amount = track_ids_size / sizeof(uint64_t);
    uint64_t *track_ids = malloc(track_ids_size);
    get_album_track_ids(track_ids, tracks_amount, album_id);
    for (uint64_t i=0; i<tracks_amount; i++) {
        Track selected = get_track(track_ids[i]);
        display_track(selected, !i);
    }
    free(track_ids);
}

void show_select_dialog() {
    printf(
        "%d - SELECT_ALL_ALBUMS\n"
        "%d - SELECT_ALL_TRACKS\n"
        "%d - SELECT_ALBUM_BY_ID\n"
        "%d - SELECT_TRACK_BY_ID\n"
        "%d - SELECT_ALBUM_TRACKS_BY_ID\n",
        SELECT_ALL_ALBUMS, 
        SELECT_ALL_TRACKS, 
        SELECT_ALBUM_BY_ID, 
        SELECT_TRACK_BY_ID,
        SELECT_ALBUM_TRACKS
    );
    int menu_choice = INVALID_MENU_OPTION;
    uint64_t chosen_id = 0;
    printf("YOUR CHOICE: ");
    scanf("%d", &menu_choice);
    switch(menu_choice) {
        case SELECT_ALL_ALBUMS:
            display_all_albums();
            break;
        case SELECT_ALL_TRACKS:
            display_all_tracks();
            break;
        case SELECT_ALBUM_BY_ID:
            printf("ALBUM_ID: ");
            scanf("%d", &chosen_id);
            display_album(get_album(chosen_id), true);
            break;
        case SELECT_TRACK_BY_ID:
            printf("TRACK_ID: ");
            scanf("%d", &chosen_id);
            display_track(get_track(chosen_id), true);
            break;
        case SELECT_ALBUM_TRACKS:
            printf("ALBUM_ID: ");
            scanf("%d", &chosen_id);
            display_album_tracks(chosen_id);
            break;
        default:
            printf("TRY AGAIN\n");
            break;
    }
}

void show_insert_dialog() {
    printf(
        "%d - INSERT_ALBUM\n"
        "%d - INSERT_TRACK\n",
        ALBUM_OPTION,
        TRACK_OPTION
    );
    int menu_choice = INVALID_MENU_OPTION;
    printf("YOUR CHOICE: ");
    scanf("%d", &menu_choice);
    Track new_track;
    Album new_album;
    switch(menu_choice) {
        case TRACK_OPTION:
            new_track.id = get_tracks_records_amount();

            printf("TRACK_ALBUM_ID: ");
            scanf(" %d", &new_track.album_id);
            printf("TRACK_NAME: ");
            scanf(" %[^\n]s", &new_track.name);
            printf("ARTIST_NAME: ");
            scanf(" %[^\n]s", &new_track.artist_name);
            printf("TRACK_LENGTH_SECONDS: ");
            scanf(" %d", &new_track.length_in_seconds);
            insert_track(new_track);     
            break;
        case ALBUM_OPTION:
            new_album.id = get_albums_records_amount();
            printf("ALBUM_RELEASE_DATE: ");
            scanf(" %d", &new_album.release_date);
            printf("ALBUM_NAME: ");
            fflush(stdin);
            scanf(" %[^\n]s", &new_album.name);
            insert_album(new_album);
            break;
        default:
            printf("TRY AGAIN\n");
            break;
    }
}

void show_update_dialog() {
    printf(
        "%d - UPDATE_ALBUM\n"
        "%d - UPDATE_TRACK\n",
        ALBUM_OPTION,
        TRACK_OPTION
    );

    char default_string_buf[64];
    uint64_t new_uint64 = 0;
    int menu_choice = INVALID_MENU_OPTION;
    int submenu_field_choice = INVALID_MENU_OPTION;
    uint64_t chosen_id = 0;
    printf("YOUR CHOICE: ");
    scanf("%d", &menu_choice);
    switch(menu_choice) {
        case ALBUM_OPTION:
            printf("ALBUM_ID: ");
            scanf("%d", &chosen_id);
            printf(
                "FIELDS TO UPDATE:\n"
                "%d - ALBUM_NAME\n"
                "%d - RELEASE_DATE\n",
                ALBUM_NAME,
                RELEASE_DATE
            );
            printf("YOUR CHOICE: ");
            scanf("%d", &submenu_field_choice);
            switch(submenu_field_choice) {
                case ALBUM_NAME:
                    printf("NEW_VALUE: ");
                    scanf("%[^\n]s", &default_string_buf);
                    update_album(chosen_id, ALBUM_NAME, default_string_buf);
                    break;
                case RELEASE_DATE:
                    printf("NEW_VALUE: ");
                    scanf("%d", &new_uint64);
                    update_album(chosen_id, RELEASE_DATE, &new_uint64);
                    break;
                default:
                    printf("TRY AGAIN\n");
                    break;
            }
            break;
        case TRACK_OPTION:
            printf("TRACK_ID: ");
            scanf("%d", &chosen_id);
            printf(
                "FIELDS TO UPDATE:\n"
                "%d - TRACK_NAME\n"
                "%d - ARTIST_NAME\n"
                "%d - TRACK_LENGTH_IN_SECONDS\n",
                TRACK_NAME,
                ARTIST_NAME,
                LENGTH_IN_SECONDS
            );
            printf("YOUR CHOICE: ");
            scanf("%d", &submenu_field_choice);
            switch(submenu_field_choice) {
                case TRACK_NAME:
                    printf("NEW_VALUE: ");
                    scanf("%[^\n]s", &default_string_buf);
                    update_track(chosen_id, TRACK_NAME, default_string_buf);
                    break;
                case ARTIST_NAME:
                    printf("NEW_VALUE: ");
                    scanf("%[^\n]s", &default_string_buf);
                    update_track(chosen_id, TRACK_NAME, default_string_buf);
                    break;
                case LENGTH_IN_SECONDS:
                    printf("NEW_VALUE: ");
                    scanf("%d", &new_uint64);
                    update_track(chosen_id, LENGTH_IN_SECONDS, &new_uint64);
                    break;
                default:
                printf("TRY AGAIN\n");
                break;
            }
            break;
        default:
            printf("TRY AGAIN\n");
            break;
    }
}

void show_delete_dialog() {
    printf(
        "%d - DELETE_ALBUM\n"
        "%d - DELETE_TRACK\n",
        ALBUM_OPTION,
        TRACK_OPTION
    );
    int menu_choice = INVALID_MENU_OPTION;
    uint64_t chosen_id = 0;
    printf("YOUR CHOICE: ");
    scanf("%d", &menu_choice);
    switch(menu_choice) {
        case ALBUM_OPTION:
            printf("ALBUM_ID: ");
            scanf("%d", &chosen_id);
            delete_album(chosen_id);
            break;
        case TRACK_OPTION:
            printf("TRACK_ID: ");
            scanf("%d", &chosen_id);
            delete_track(chosen_id);
            break;
        default:
            printf("TRY AGAIN\n");
            break;
    }
}

void show_main_dialog() {
    printf(
        "OPERATIONS:\n"
        "%d - SELECT\n"
        "%d - INSERT\n"
        "%d - UPDATE\n"
        "%d - DELETE\n",
        SELECT,
        INSERT,
        UPDATE,
        DELETE
    );
    int menu_choice = INVALID_MENU_OPTION;
    printf("YOUR CHOICE: ");
    scanf("%d", &menu_choice);
    switch(menu_choice) {
        case SELECT:
            show_select_dialog();
            break;
        case INSERT:
            show_insert_dialog();
            break;
        case UPDATE:
            show_update_dialog();
            break;
        case DELETE:
            show_delete_dialog();
            break;
        default:
            printf("TRY AGAIN\n");
            break;
    }
}

int main() {
    printf("Hello world!!1\n\n");
    while (228) {
        show_main_dialog();
    }
    return 0;
}
