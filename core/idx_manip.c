#include <stdio.h>
#include<stdlib.h>
#include <string.h>
#include <stdint.h>

#include "data_types.h"
#include "util.h"
#include "idx_manip.h"

#define UINT_64_MAX_LEN 20
#define ALBUMS_IDX_FNAME_MAX_LEN strlen(ALBUMS_IDX_FNAME_TEMPLATE) - strlen("%d") + UINT_64_MAX_LEN

const char *ALBUMS_FNAME = "db_data/albums.fl";
const char *ALBUMS_IDX_FNAME_TEMPLATE = "db_data/idx/album_idx-%d.idx";
const char *TRACKS_FNAME = "db_data/tracks.fl";


uint64_t get_albums_records_amount() { 
    return get_fsize(ALBUMS_FNAME) / sizeof(struct Album);
}

uint64_t get_tracks_records_amount() {
    return get_fsize(TRACKS_FNAME) / sizeof(struct Track);
}

size_t get_album_idx_size(uint64_t album_id) {
    char current_album_idx_fname[ALBUMS_IDX_FNAME_MAX_LEN];
    sprintf(current_album_idx_fname, ALBUMS_IDX_FNAME_TEMPLATE, album_id);
    return get_fsize(current_album_idx_fname);
}

void get_album_track_ids(uint64_t *ids_buf, uint64_t amount, uint64_t album_id) {
    FILE *fp_tracks = fopen(TRACKS_FNAME, "rb");
    if (fp_tracks == NULL) {
        return;
    }
    char current_album_idx_fname[ ALBUMS_IDX_FNAME_MAX_LEN ];
    sprintf(current_album_idx_fname, ALBUMS_IDX_FNAME_TEMPLATE, album_id);
    FILE *fp_cur_album_idx = fopen(current_album_idx_fname, "rb");
    fread(ids_buf, sizeof(uint64_t), amount, fp_cur_album_idx);
    fclose(fp_cur_album_idx);
}


Album get_album(uint64_t album_id) {
    FILE *fp_albums = fopen(ALBUMS_FNAME, "rb");
    Album album;
    uint64_t record_size = sizeof(album);
    uint64_t record_pos_offset = album_id * record_size;
    fpread(&album, record_size, 1, record_pos_offset, fp_albums);
    fclose(fp_albums);
    return album;
}

Track get_track(uint64_t track_id) {
    FILE *fp_tracks = fopen(TRACKS_FNAME, "rb");
    Track record;
    uint64_t record_pos_offset = track_id * sizeof(record);
    fpread(&record, sizeof(record), 1, record_pos_offset, fp_tracks);
    fclose(fp_tracks);
    return record;
}


void update_album(uint64_t id, AlbumFields field, void *new_val) {
    Album record = get_album(id);
    switch(field) {
        case ALBUM_NAME:
            strcpy(record.name, (const char*)new_val);
            break;
        case RELEASE_DATE:
            record.release_date = *((uint64_t*)(new_val));
            break;
        default:
            return;
    }
    FILE *fp = fopen(ALBUMS_FNAME, "r+");
    if (fp == NULL) {
        return;
    }
    uint64_t record_pos_offset = id * sizeof(record);
    fpwrite(&record, sizeof(record), 1, record_pos_offset, fp);
    fclose(fp);
}

void update_track(uint64_t id, TrackFields field, void *new_val) {
    Track record = get_track(id);
    switch(field) {
        case TRACK_NAME:
            strcpy(record.name, (const char*)new_val);
            break;
        case ARTIST_NAME:
            strcpy(record.artist_name, (const char*)new_val);
            break;
        case LENGTH_IN_SECONDS:
            record.length_in_seconds = *((uint64_t*)(new_val));
            break;
        default:
            return;
    }
    FILE *fp = fopen(TRACKS_FNAME, "rb+");
    if (fp == NULL) {
        return;
    }
    uint64_t record_pos_offset = id * sizeof(record);
    fpwrite(&record, sizeof(record), 1, record_pos_offset, fp);
    fclose(fp);
}


void insert_album(Album album) {
    FILE *fp_albums = fopen(ALBUMS_FNAME, "ab");
    if (fp_albums == NULL) {
        return;
    }
    fwrite(&album, sizeof(struct Album), 1, fp_albums);
    fclose(fp_albums);
}

void insert_track(Track track) {
    FILE *fp_tracks = fopen(TRACKS_FNAME, "ab");
    if (fp_tracks == NULL) {
        return;
    }
    fseek(fp_tracks, 0L, SEEK_END);
    fwrite(&track, sizeof(struct Track), 1, fp_tracks);
    fclose(fp_tracks);
    char current_album_fname[ALBUMS_IDX_FNAME_MAX_LEN];
    sprintf(current_album_fname, ALBUMS_IDX_FNAME_TEMPLATE, track.album_id);
    FILE *fp_current_album_idx = fopen(current_album_fname, "ab");
    fwrite(&track.id, sizeof(track.id), 1, fp_current_album_idx);
    fclose(fp_current_album_idx);
}

void delete_record(uint64_t record_id, size_t rec_size, const char *fname) {
    size_t records_fsize = get_fsize(fname);
    FILE *fp_records = fopen(fname, "rb+");
    if (fp_records == NULL) {
        return;
    }

    size_t before_target_size = record_id * rec_size;
    size_t after_target_size = records_fsize - before_target_size - rec_size;
    char *before_target_buf = malloc(before_target_size);
    char *after_target_buf = malloc(after_target_size);
    fseek(fp_records, 0L, SEEK_SET);
    fread(before_target_buf, before_target_size, 1, fp_records);
    fseek(fp_records, rec_size, SEEK_CUR);
    fread(after_target_buf, after_target_size, 1, fp_records);
    fclose(fp_records);
    remove(fname);

    for (uint64_t i=1; i<records_fsize; i+=rec_size) {
        uint64_t record_id;
        memcpy(&record_id, &after_target_buf[i], sizeof(record_id));
        record_id -= 1;
        memcpy(&after_target_buf[i], &record_id, sizeof(record_id));
    }

    FILE *fp_records_new = fopen(fname, "wb");
    fwrite(before_target_buf, before_target_size, 1, fp_records_new);
    fwrite(after_target_buf, after_target_size, 1, fp_records_new);
    free(before_target_buf);
    free(after_target_buf);
    fclose(fp_records_new);
}

void delete_album(uint64_t album_id) {
    // Removing all records that current index points to
    size_t album_idx_size = get_album_idx_size(album_id);
    uint64_t *album_idx_track_ids = malloc(album_idx_size);
    get_album_track_ids(album_idx_track_ids, album_idx_size / sizeof(uint64_t), album_id);
    for (uint64_t i=0; i<album_idx_size / sizeof(uint64_t); i++) {
        uint64_t track_id = album_idx_track_ids[i];
        delete_track(track_id - i);
    }
    free(album_idx_track_ids);

    // Removing the index itself
    char album_idx[ALBUMS_IDX_FNAME_MAX_LEN];
    sprintf(album_idx, ALBUMS_IDX_FNAME_TEMPLATE, album_id);
    remove(album_idx);
    
    // Reindex 
    uint64_t records_amount = get_fsize(ALBUMS_FNAME) / sizeof(struct Album);
    for (int i=album_id+1; i<records_amount; i++) {
        char current_idx_fname[ALBUMS_IDX_FNAME_MAX_LEN];
        char new_idx_fname[ALBUMS_IDX_FNAME_MAX_LEN];
        sprintf(current_idx_fname, ALBUMS_IDX_FNAME_TEMPLATE, i);
        sprintf(new_idx_fname, ALBUMS_IDX_FNAME_TEMPLATE, i-1);
        rename(current_idx_fname, new_idx_fname);
    }

    //Removing album itself
    delete_record(album_id, sizeof(struct Album), ALBUMS_FNAME);
}

void delete_track(uint64_t track_id) {
    // Removing track from album index
    Track track = get_track(track_id);
    size_t album_idx_size = get_album_idx_size(track.album_id);
    uint64_t album_idx_len = album_idx_size / sizeof(uint64_t);
    uint64_t *album_idx_track_ids = malloc(album_idx_size);
    get_album_track_ids(album_idx_track_ids, album_idx_len, track.album_id);
    uint64_t track_index_pos = uint64_index_of(album_idx_track_ids, album_idx_len, track_id);
    char album_idx_fname[ALBUMS_IDX_FNAME_MAX_LEN];
    sprintf(album_idx_fname, ALBUMS_IDX_FNAME_TEMPLATE, track.album_id);
    delete_record(track_index_pos, sizeof(uint64_t), album_idx_fname);

    //Removing track itself
    delete_record(track_id, sizeof(struct Track), TRACKS_FNAME);
}
