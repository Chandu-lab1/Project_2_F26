#include <dirent.h>
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

/* Turns the permission bits into a string such as rwxr-xr-x. */
void get_permissions(mode_t mode, char permissions[])
{
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[9] = '\0';
}

/* Gets the metadata for one entry and prints it on one line. */
void print_entry(const char *directory_path, const char *file_name)
{
    char full_path[PATH_MAX];
    char permissions[10];
    struct stat file_info;
    const char *file_type;
    int path_length;

    /* readdir gives only a name, so add the directory before using stat. */
    if (strcmp(directory_path, "/") == 0) {
        path_length = snprintf(full_path, sizeof(full_path), "%s%s",
                               directory_path, file_name);
    } else {
        path_length = snprintf(full_path, sizeof(full_path), "%s/%s",
                               directory_path, file_name);
    }

    /* A path that does not fit cannot be safely passed to stat. */
    if (path_length < 0 || path_length >= (int)sizeof(full_path)) {
        return;
    }

    /* The project says to skip an entry if stat fails. */
    if (stat(full_path, &file_info) == -1) {
        return;
    }

    /* S_ISDIR checks the file-type bits stored inside st_mode. */
    if (S_ISDIR(file_info.st_mode)) {
        file_type = "[DIR]";
    } else {
        file_type = "[FILE]";
    }

    /* The same st_mode value also stores the nine permission bits. */
    get_permissions(file_info.st_mode, permissions);

    /* The columns are type, permissions, size in bytes, and entry name. */
    printf("%-6s %-9s %10lld  %s\n", file_type, permissions,
           (long long)file_info.st_size, file_name);
}

/* Opens one directory and goes through each entry inside it. */
int list_directory(const char *directory_path, int show_path_heading)
{
    DIR *directory;
    struct dirent *entry;

    directory = opendir(directory_path);

    if (directory == NULL) {
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    /* Print this heading only after the requested directory opened. */
    if (show_path_heading) {
        printf("[[%s]]\n\n", directory_path);
    }

    /* readdir returns one entry at a time and includes . and .. */
    while (1) {
        errno = 0;
        entry = readdir(directory);

        if (entry == NULL) {
            break;
        }

        print_entry(directory_path, entry->d_name);
    }

    /* NULL normally means the end, but errno tells us if scanning failed. */
    if (errno != 0) {
        closedir(directory);
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    if (closedir(directory) == -1) {
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    /* No argument means list only the current directory. */
    if (argc == 1) {
        return list_directory(".", 0);
    }

    /* This program accepts no more than one directory argument. */
    if (argc != 2) {
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    if (list_directory(argv[1], 1) == 1) {
        return 1;
    }

    printf("\nListing: current directory\n\n");

    if (list_directory(".", 0) == 1) {
        return 1;
    }

    return 0;
}
/*
 * Project 2: The Enhanced Directory Explorer (tumls)
 *
 * This program works like a simple version of ls. It uses opendir() to open
 * a directory and readdir() to get the names inside it. For every name, it
 * creates the full path and passes that path to stat(). The stat structure
 * gives the program the file type, permission bits, and size in bytes.
 *
 * stat() follows symbolic links, which is required for this project. If stat()
 * fails for one entry, that entry is skipped and the program keeps going.
 */
