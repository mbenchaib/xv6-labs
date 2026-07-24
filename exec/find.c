#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"
#include "kernel/fcntl.h"

static void 
getname(const char *path, char *name)
{
    const char *p;
    
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
    p++;
    
    for (int i = 0; i < DIRSIZ && *(p + i); i++)
        name[i] = *(p + i);
}

static int
match(const char *fileName, const char *name)
{
    int i;
    for (i = 0; (fileName[i] == name[i] && fileName[i] && i < DIRSIZ); i++)
    ;
    return (fileName[i] - name[i]);
}

static int
joinpath(const char *path, const char *name, char *fullPath)
{
    int len_name;
    for (len_name = 0; len_name < DIRSIZ && name[len_name]; len_name++)
    ;

    int len_path;
    for (len_path = 0; len_path < MAXPATH && path[len_path]; len_path++)
    ;

    if (len_path + len_name > MAXPATH)
        return (1);

    memmove(fullPath, path, len_path);
    fullPath[len_path] = '/';
    memmove(fullPath + len_path + 1, name, len_name);
    return (0);
}

int
append_file_arg(char *path, char **cmd)
{
    int i = 0;
    for (i = 0; i < MAXARG && cmd[i]; i++)
    ;
    if (i == MAXARG)
        return (1);
    cmd[i] = path;
    return (0); 
}

void
find(char *path, char *fileName, char **cmd)
{
    int             fd;
    struct dirent   de;
    struct stat     st;
    char            name[DIRSIZ] = {0};
    char            fullPath[MAXPATH + 1] = {0};

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }
    
    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch(st.type)
    {   
        case T_FILE:
            getname(path, name);
            if (!match(fileName, name))
            {
                if (cmd[0] != 0)
                {
                    int pid = fork();
                    if (pid == -1)
                    {
                        fprintf(2, "find: fork failed\n");
                        close(fd);
                        return;
                    }
                    if (pid == 0)
                    {
                        if (append_file_arg(path, cmd))
                        {
                            fprintf(2, "find: too many arguments\n");
                            exit(1);
                        }
                        if (exec(cmd[0], cmd) == -1)
                        {
                            fprintf(2, "find: exec failed\n");
                            exit(1);
                        }
                        exit(0);
                    }
                    else
                    {
                        int status;
                        wait(&status);
                    }
                }
                else
                    printf("%s\n", path);
            }
            close(fd);
            break ;
        case T_DIR:
            while(read(fd, &de, sizeof(de)) == sizeof(de))
            {
                memset(fullPath, '\0', MAXPATH + 1);
                if (de.inum == 0 || (de.name[0] == '.' && de.name[1] == '\0') || (de.name[0] == '.' && de.name[1] == '.' && de.name[2] == '\0'))
                    continue ;

                if (joinpath(path, de.name, fullPath))
                {
                    fprintf(2, "find: path too long\n");
                    close(fd);
                    return;
                }
                find(fullPath, fileName, cmd);
            }
            break ;
        close(fd);
    }
}

int
main(int argc, char *argv[])
{
    char *cmd[MAXARG] = {0};

    if (argc - 4 > MAXARG)
    {
        fprintf(2, "find: too many arguments\n");
        exit(1);
    }
    if (argc < 3 || (argv[3] != 0 && strcmp(argv[3], "-exec")))
    {
        fprintf(2, "find: usage: find <path> <name> [-exec cmd [args...]]\n");
        exit(1);
    }

    for (int i = 4; argv[3] != 0 && argv[i] != 0; i++)
        cmd[i - 4] = argv[i];

    find(argv[1], argv[2], cmd);
    exit(0);
}
