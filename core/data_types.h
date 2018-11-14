#pragma once

#include <stdint.h>

typedef struct Album {
    uint64_t id;

    char name[64];
    uint64_t release_date;
} Album;

typedef enum {
    ALBUM_NAME,
    RELEASE_DATE
} AlbumFields;


typedef struct Track {
    uint64_t id;
    uint64_t album_id;

    char name[64];
    char artist_name[64];
    uint32_t length_in_seconds;
} Track;

typedef enum TrackFields {
    TRACK_NAME,
    ARTIST_NAME,
    LENGTH_IN_SECONDS
} TrackFields;
