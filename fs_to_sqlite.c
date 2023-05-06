#include <dirent.h>
#include <stdio.h>
#include <sqlite3.h>
#include <sys/stat.h>
#include <time.h>

// Change the schema in the create_table function
void create_table(sqlite3 *db) {
    char *err_msg = 0;
    char *sql = "CREATE TABLE IF NOT EXISTS files (path TEXT PRIMARY KEY, bytes INTEGER NOT NULL, mtime REAL NOT NULL);";

    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);

    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot create table: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
}

// Change the insert_file function to match the schema
void insert_file(sqlite3 *db, const char *path, off_t size, time_t mtime) {
    sqlite3_stmt *stmt;
    char *sql = "INSERT INTO files (path, bytes, mtime) VALUES (?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, path, -1, SQLITE_STATIC);
        sqlite3_bind_int64(stmt, 2, size);
        sqlite3_bind_double(stmt, 3, (double)mtime); // Change this line to store mtime as REAL

        if (sqlite3_step(stmt) != SQLITE_DONE) {
            fprintf(stderr, "Failed to insert file: %s\n", sqlite3_errmsg(db));
        }

        sqlite3_finalize(stmt);
    }
}


void scan_directory(const char *dir_path, sqlite3 *db) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "Error opening directory: %s\n", dir_path);
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
                char sub_dir[4096];
                snprintf(sub_dir, sizeof(sub_dir), "%s/%s", dir_path, entry->d_name);
                scan_directory(sub_dir, db);
            }
        } else {
            char file_path[4096];
            snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, entry->d_name);

            struct stat sb;
            if (stat(file_path, &sb) == 0) {
                insert_file(db, file_path, sb.st_size, sb.st_mtime);
            }
        }
    }

    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <directory> <sqlite_db>\n", argv[0]);
        return 1;
    }

    sqlite3 *db;
    if (sqlite3_open(argv[2], &db) != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        return 1;
    }

    create_table(db);
    scan_directory(argv[1], db);

    sqlite3_close(db);

    return 0;
}
