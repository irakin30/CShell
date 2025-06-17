#include "shell.h"
int main(){
    char *line = malloc(1000);
    char *current = malloc(1000);
    char *temp = malloc(1000);
    char **args = malloc(20 * sizeof(char*));
    char **args2 = malloc(20 * sizeof(char*));
    char **cat = malloc(4 * sizeof(char*));
    cat[0] = "cat";
    cat[1] = "tmp";
    cat[2] = NULL;
    //cat array just for catting tmp into another file with exec lmao
    char **arglist = malloc(20 * sizeof(char*));
    int arglisti;
    char **oplist = malloc(20 * sizeof(char*));
    int oplisti;
    char* user = getenv("USER");
    pid_t child;
    while(1){
        char dir[1024];
        getcwd(dir, 1024);
        printf("JingleShell (%s)$ ", dir);
        fgets(line, 1000, stdin);
        strcat(line, ";");
        while (strchr(line, ';')){
            arglisti = 0;
            oplisti = 0;
            current = strsep(&line, ";"); //separates the next command line
            current = strip_spaces(current);
            /*
            this next bit separates argument pieces from operators (the funny symbols)
            it finishes with one array of str of args and one array of str of operators
            istab has begged me to turn this into a function but i think thats kinda cumbersome and cringe -Lee
            */
            for (int x = 0; x < strlen(current)-1; x++){
                if (current[x] == '<' || current[x] == '|'){
                    current[x-1] = '\0';
                    arglist[arglisti] = current;
                    arglisti++;
                    current[x+1] = '\0';
                    oplist[oplisti] = &current[x];
                    oplisti++;
                    current+=x+2;
                    x = 0;
                }
                if (current[x] == '>'){
                    if (current[x+1] == '>'){
                        current[x-1] = '\0';
                        arglist[arglisti] = current;
                        arglisti++;
                        current[x+2] = '\0';
                        oplist[oplisti] = &current[x];
                        oplisti++;
                        current+=x+3;
                        x = 0;
                    } else{
                        current[x-1] = '\0';
                        arglist[arglisti] = current;
                        arglisti++;
                        current[x+1] = '\0';
                        oplist[oplisti] = &current[x];
                        oplisti++;
                        current+=x+2;
                        x = 0;
                    }
                }
            }
            arglist[arglisti] = current;
            arglisti++;
            if (arglisti == 1){ //if one command
                args = parse_args(arglist[0]);
                run(args);
            } else if (oplisti == 1){ //if one operator
                args = parse_args(arglist[0]);
                if (!strcmp(oplist[0], "<")){
                    redirect_in(arglist[1], args);
                } else if (!strcmp(oplist[0], "> ")){
                    redirect_out(arglist[1], args);
                } else if (!strcmp(oplist[0], ">>")){
                    redirect_out_append(arglist[1], args);
                } else if (!strcmp(oplist[0], "|")){
                    args2 = parse_args(arglist[1]);
                    redirect_pipe(args, args2);
                }
            } else{
                //only for multiple operators
                //first operation
                args = parse_args(arglist[0]);
                if (!strcmp(oplist[0], "<")){
                    redirect_inout(arglist[1], args, "tmp");
                } else if (!strcmp(oplist[0], "|")){
                    args2 = parse_args(arglist[1]);
                    redirect_out("tmp2",args);
                    redirect_inout("tmp2",args2,"tmp");
                    child = fork();
                    if (child == 0) {
                        execlp("rm", "rm", "tmp2", NULL); //deletes temp2 file
                    } else {
                        waitpid(child, NULL, 0); 
                    }
                }

                //all middle operations should be pipes lol
                for (int x = 1; x < oplisti-1; x++){
                    if (!strcmp(oplist[x], "|")){
                        args = parse_args(arglist[x+1]);
                        redirect_out("tmp2",cat);
                        redirect_inout("tmp2",args,"tmp");
                        child = fork();
                        if (child == 0) {
                            execlp("rm", "rm", "tmp2", NULL); //deletes temp2 file
                        } else {
                            waitpid(child, NULL, 0); 
                        }
                    }
                }

                //last operation
                if (!strcmp(oplist[oplisti-1], "> ")){
                    // execlp("cp", "cp", "tmp", arglist[arglisti-1], NULL); //copies tmp to output file
                    //alternate method
                    redirect_out(arglist[arglisti-1], cat);
                } else if (!strcmp(oplist[oplisti-1], ">>")){
                    redirect_out_append(arglist[arglisti-1], cat); //appends cat tmp to output file
                } else if (!strcmp(oplist[oplisti-1], "|")){
                    args = parse_args(arglist[arglisti-1]);
                    redirect_in("tmp",args);
                }
                child = fork();
                if (child == 0) {
                    execlp("rm", "rm", "tmp", NULL); //deletes temp file
                } else {
                    waitpid(child, NULL, 0); 
                }
            }
        }
    }
}
