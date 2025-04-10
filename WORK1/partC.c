// Names + Id: Eliran Ovadia 206753592
//  	       Ofir Almog 207918731
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

void getWords(char list[][50], char *filename)
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        perror("Error opening the file");
        exit(-1);
    }
    int word_index = 0;
    int char_index = 0;
    while ((bytes_read = read(file_descriptor, buffer, BUFFER_SIZE)) > 0)
    {
        for (ssize_t i = 0; i < bytes_read; i++)
        {
            if (buffer[i] == ' ')
            {
                i++;
                list[word_index][char_index] = '\0';
                word_index++;
                char_index = 0;
            }
            list[word_index][char_index] = buffer[i];
            char_index++;
        }
    }
    if (bytes_read == -1)
    {
        perror("Error reading the file");
        exit(-1);
    }
    close(file_descriptor);
}
int getCount(char list[][50], char *filename)
{
    char buffer[BUFFER_SIZE];
    int count = 0;
    ssize_t bytes_read;

    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == -1)
    {
        perror("Error opening the file");
        exit(-1);
    }
    int word_index = 0;
    int char_index = 0;
    while ((bytes_read = read(file_descriptor, buffer, BUFFER_SIZE)) > 0)
    {
        for (ssize_t i = 0; i < bytes_read; i++)
        {
            if (buffer[i] == '\n')
            {
                count++;
                i++;
                list[word_index][char_index] = '\0';
                word_index++;
                char_index = 0;
            }
            list[word_index][char_index] = buffer[i];
            char_index++;
        }
    }
    if (bytes_read == -1)
    {
        perror("Error reading the file");
        exit(-1);
    }
    close(file_descriptor);
    return count;
}
void executeCommand(char *command, char *args[], char *filename)
{
    int file_descriptor;
    int pid;
    int screen = dup(1);
    pid = fork();
    if (pid < 0)
    {
        exit(-1);
    }
    if (pid == 0)
    {
        file_descriptor = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
        if (file_descriptor == -1)
        {
            close(-1);
        }
        close(1);
        dup(file_descriptor);
        close(file_descriptor);
        int ret_code = execvp(command, args);
        if (ret_code == -1)
        {
            perror("exec failed ");
            exit(-1);
        }
    }
    else
    {
        wait(NULL);
    }
}
char *buildCommandPath(char *config, char *name)
{
    char *path = malloc(BUFFER_SIZE);
    int i = 0;
    int j = 0;
    char *exe = "main.exe";
    char *c = "/";
    while (*config)
    {
        path[i] = *config;
        config++;
        i++;
    }
    path[i] = *c;
    i++;
    while (*name)
    {
        path[i] = *name;
        i++;
        name++;
    }
    path[i] = *c;
    i++;
    while (*exe)
    {
        path[i] = *exe;
        i++;
        exe++;
    }
    path[i] = '\0';
    return path;
}
int main(int argc, char *argv[])
{
    int file_descriptor;
    char config[3][50];
    char names[100][50];
    char inputs[2][50];
    pid_t pid;
    file_descriptor = open("grades.csv", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (file_descriptor == -1)
    {
        perror("Error opening the file");
        exit(-1);
    }

    char *config_file = argv[1];
    int num_configs = getCount(config, config_file);
    char *ls_command = "ls";
    char *ls_args[] = {"ls", config[0], NULL};
    executeCommand(ls_command, ls_args, "studList.txt");
    char *studList_file = "studList.txt";
    int num_students = getCount(names, studList_file);
    getWords(inputs, config[1]);
    for (int i = 0; i < num_students; i++)
    {
        char *command_path = buildCommandPath(config[0], names[i]);
        char *execution_args[] = {command_path, inputs[0], inputs[1], NULL};
        executeCommand(command_path, execution_args, "out.txt");
        char *comp_args[] = {"./comp.exe", "out.txt", config[2], NULL};
        int comp_pipe[2];
        if (pipe(comp_pipe) == -1)
        {
            perror("Pipe error");
            exit(-1);
        }

        int comp_pid = fork();
        if (comp_pid < 0)
        {
            perror("Fork error");
            exit(-1);
        }
        else if (comp_pid == 0)
        {
            close(comp_pipe[1]);
            dup2(comp_pipe[0], STDIN_FILENO);
            close(comp_pipe[0]);

            int ret_code = execvp("./comp.exe", comp_args);
            if (ret_code == -1)
            {
                perror("Exec failed");
                exit(-1);
            }
        }
        else
        {
            close(comp_pipe[0]);
            int comp_result = open("comp_result.txt", O_RDONLY);
            if (comp_result == -1)
            {
                perror("Error opening the file");
                exit(-1);
            }

            char comp_buffer[BUFFER_SIZE];
            ssize_t bytes_read = read(comp_result, comp_buffer, BUFFER_SIZE);
            close(comp_result);

            write(comp_pipe[1], comp_buffer, bytes_read);
            close(comp_pipe[1]);

            int status_comp;
            waitpid(comp_pid, &status_comp, 0);

            if (WIFEXITED(status_comp))
            {
                int exit_status_comp = WEXITSTATUS(status_comp);
                int grade = (exit_status_comp == 2) ? 100 : 0;
                dprintf(file_descriptor, "%s: %d\n", names[i], grade);
            }
        }
        free(command_path);
    }
    close(file_descriptor);
    return 0;
}

