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

void
find(const char *path, const char *fileName)
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
                printf("%s\n", path);
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
                find(fullPath, fileName);
            }
            break ;
        close(fd);
    }
}

int
main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "find: usage: find <path> <fileName>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}