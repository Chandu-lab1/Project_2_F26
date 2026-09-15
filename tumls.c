/*
 * Project 2 - The Enhanced Directory Explorer: tumls
 *
 * This program is a simple version of the Linux ls command.
 * It opens a directory and prints information about every entry.
 * The information includes the file type, permissions, size in
 * bytes, and filename.
 *
 * opendir() opens the directory.
 * readdir() gets one entry from the directory at a time.
 * stat() gets the metadata for each entry.
 * closedir() closes the directory when the program is finished.
 *
 * stat() follows symbolic links, as required by this project. If
 * stat() fails for one entry, my program skips it and keeps going.
 */

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <sys/stat.h>

/* Change the permission bits into a string such as rwxr-xr-x. */
void get_permissions(mode_t mode, char permissions[])
{
    /* The & operator checks if each permission bit is turned on. */

    /* These three characters are the owner's permissions. */
    permissions[0] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[1] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[2] = (mode & S_IXUSR) ? 'x' : '-';

    /* These three characters are the group's permissions. */
    permissions[3] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[4] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[5] = (mode & S_IXGRP) ? 'x' : '-';

    /* These three characters are everyone else's permissions. */
    permissions[6] = (mode & S_IROTH) ? 'r' : '-';
    permissions[7] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[8] = (mode & S_IXOTH) ? 'x' : '-';

    /* A C string needs a null character at the end. */
    permissions[9] = '\0';
}

/* Get and print the metadata for one directory entry. */
void print_entry(const char *directory_path, const char *file_name)
{
    char full_path[4096];
    char permissions[10];
    struct stat file_info;
    const char *file_type;
    int path_length;

    /*
     * readdir() only gives the entry name. Add the directory path
     * so stat() looks for the entry in the correct directory.
     */
    path_length = snprintf(full_path, sizeof(full_path), "%s/%s",
                           directory_path, file_name);

    /* Skip the entry if its full path does not fit in the array. */
    if (path_length < 0 || path_length >= (int)sizeof(full_path))
    {
        return;
    }

    /* The project says to skip one entry if stat() fails. */
    if (stat(full_path, &file_info) == -1)
    {
        return;
    }

    /* S_ISDIR checks the file type bits inside st_mode. */
    if (S_ISDIR(file_info.st_mode))
    {
        file_type = "[DIR]";
    }
    else
    {
        /* Other file types are displayed as [FILE] for this project. */
        file_type = "[FILE]";
    }

    /* st_mode also contains the nine permission bits. */
    get_permissions(file_info.st_mode, permissions);

    /* Print type, permissions, size in bytes, and filename. */
    printf("%-6s %-9s %10lld  %s\n", file_type, permissions,
           (long long)file_info.st_size, file_name);
}

/* Open one directory and go through every entry inside it. */
int list_directory(const char *directory_path, int show_path_heading)
{
    DIR *directory;
    struct dirent *entry;

    directory = opendir(directory_path);

    /* NULL means opendir() could not open the directory. */
    if (directory == NULL)
    {
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    /* Only show this heading when the user gives a directory path. */
    if (show_path_heading)
    {
        printf("[[%s]]\n\n", directory_path);
    }

    while (1)
    {
        errno = 0;

        /* Ask the operating system for the next directory entry. */
        entry = readdir(directory);

        /* NULL normally means there are no more entries. */
        if (entry == NULL)
        {
            break;
        }

        /* d_name contains the name of the current entry. */
        print_entry(directory_path, entry->d_name);
    }

    /* errno will not be 0 if readdir() failed while scanning. */
    if (errno != 0)
    {
        closedir(directory);
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    /* Always close the directory when the program is finished. */
    if (closedir(directory) == -1)
    {
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    /* argc includes the program name, so 1 means no path was given. */
    if (argc == 1)
    {
        return list_directory(".", 0);
    }

    /* This program accepts only one directory path. */
    if (argc != 2)
    {
        fprintf(stderr, "tumls: cannot open directory\n");
        return 1;
    }

    /* argv[1] is the directory path typed after ./tumls. */
    if (list_directory(argv[1], 1) != 0)
    {
        return 1;
    }

    /* The project requires the current directory as the second list. */
    printf("\nListing: current directory\n\n");

    if (list_directory(".", 0) != 0)
    {
        return 1;
    }

    return 0;
}
