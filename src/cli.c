#include "todos.c"

// Define structure for commands that require parameters or flags
// Name starts with underscore '_' means it is a parameter otherwise flag
struct ls {
    int format;
    int done;
    int todos;
    int _use;
};

struct done {
    int done__undone;
    int _use;
};

struct del {
    int all;
    int force;
    int _use;
};

int main(int argc, char const *argv[]){

    char base_command[10], all_todos[100000],
         // Stores commands that do not require the 'get_all_todos' function to be run
         preload_not_required_commands[3][7] = {"add", "use", "create"};
    int start = 0, size = sizeof(all_todos), todo_id, command_match;

    if (argc > 1){
        strcpy(base_command, argv[1]);
        command_match = in(base_command, preload_not_required_commands, 3);

        for (int i=2; i < argc; i++){
            if (!strcmp("-use", argv[i])){
                if (i+1 == argc){
                    printf("Please pass a todo file name\n");
                    exit(0);
                }
                use_file = (char *) argv[i+1];
                break;
            }
        }

        // Set the currently active file name
        set_active_file_name();

        if (!command_match){
            // Get all the todos
            get_all_todos(all_todos, size);
        }

    } else{
        printf("No command passed\n");
        return 0;
    }

    /*
        --------------- EXECUTOR COMMANDS ---------------
    */

    if (!strcmp(base_command, "add")){
        if (argc <= 3){
            printf("Priority & todo message is required\n");
        } else{
            // Check requried positional argument Todo priority
            if (argv[2][0] == '-'){
                printf("Missing required positional argument Todo priority\n");
                exit(0);
            }

            // Check requried positional argument Todo message
            if (argv[3][0] == '-'){
                printf("Missing required positional argument Todo message\n");
                exit(0);
            }

            size = argc-3;
            char todo[size][1000], priority = argv[2][0];

            // copy the todo to a new char array
            for (int i=0; i < size; i++) {
                if (argv[i+3][0] == '-'){
                    size = i;
                    break;
                }
                strcpy(todo[i], argv[i+3]);        
            }

            add_todo(priority, size, todo);
            printf("Todo added\n");
        }
    }

    else if (!strcmp(base_command, "done")){
        if (argc <= 2){
            printf("Todo id is required\n");
        } else{

            // Check if 2nd argument is starts with '-', meaning it's an flag or parameter
            if (argv[2][0] == '-'){
                printf("Missing required positional argument Todo id\n");
                exit(0);
            }

            todo_id = atoi(argv[2]);
            struct done _done;

            // assign default value
            _done.done__undone = 1;
            _done._use = 0;

            for (int i=3; i<argc; i++){
                if (!strcmp(argv[i], "--done")){
                    _done.done__undone = 1;
                } else if (!strcmp(argv[i], "--undone")){
                    _done.done__undone = 0;
                } else if (!strcmp(argv[i], "-use")){
                    _done._use = 1;
                    i++;
                } else {
                    printf("%s is not a valid flag/parameter\n", argv[i]);
                    exit(0);
                }
            }

            mark_todo(todo_id, all_todos, _done.done__undone);

        }
    }

    else if (!strcmp(base_command, "del")){
        if (argc <= 2){
            printf("Todo id is required\n");
        } else {
            struct del _del;

            // assign default value
            _del.all = 0;
            _del.force = 0;
            _del._use = 0; 

            for (int i=2; i<argc; i++){
                if (argv[i][0] != '-'){
                    continue;
                }else if (!strcmp(argv[i], "--all")){
                    _del.all = 1;
                } else if (!strcmp(argv[i], "--force")){
                    _del.force = 1;
                } else if (!strcmp(argv[i], "-use")){
                    _del._use = 1;
                    i++;
                } else {
                    printf("%s is not a valid flag/parameter\n", argv[i]);
                    exit(0);
                }
            }

            // Check if 2nd argument is starts with '-', meaning it's an flag or parameter
            if (argv[2][0] == '-' && !_del.all){
                printf("Missing required positional argument Todo id\n");
                exit(0);
            }

            if (_del.all){
                char confirmation[2];

                // take a second confirmation from the user
                printf("Please confirm(y/n) ");
                scanf("%s", confirmation);

                if (!strcmp(confirmation, "y")){
                    del_all_todos(all_todos);
                    printf("All completed todos deleted\n");
                }

            } else{
                todo_id = atoi(argv[2]);
                del_todo(todo_id, _del.force, all_todos);
            }
        }
    }

    else if (!strcmp(base_command, "report")){
        generate_report(all_todos);
    }

    else if (!strcmp(base_command, "ls")){
        struct ls _ls;

        // assign default value
        _ls.todos = 0;
        _ls.format = 0;
        _ls.done = 0;
        _ls._use = 0;

        for (int i=2; i<argc; i++){
            if (!strcmp(argv[i], "--todos")){
                _ls.todos = 1;
            } else if (!strcmp(argv[i], "--format")){
                _ls.format = 1; 
            } else if (!strcmp(argv[i], "--done")){
                _ls.done = 1; 
            } else if (!strcmp(argv[i], "-use")){
                _ls._use = 1;
                i++;
            } else {
                printf("%s is not a valid flag/parameter\n", argv[i]);
                exit(0);
            }
        }

        if (_ls.todos)
            list_all_todos_files();
        else if (_ls.done)
            list_completed_todos(all_todos, _ls.format);
        else
            list_pending_todos(all_todos, _ls.format);

    }

    else if (!strcmp(base_command, "use")){
        if (argc < 3) {
            printf("File name is missing\n");
        } else {
            char *file_name = (char *) argv[2];
            update_active_todo_file(file_name);
        }
    }

    else if (!strcmp(base_command, "create")){
        if (argc < 3) {
            printf("File name is missing\n");
        } else {
            char *file_name = (char *) argv[2];
            create_todo_file(file_name);
        }
    }

    else if (!strcmp(base_command, "active")){
        if (argc < 3) {
            char file_name[100];
            get_active_todo_file_name(file_name);
            printf("%s\n", file_name);
        } else if (!strcmp(argv[2], "--path")) {
            printf("%s\n", active_file_path);
        } else {
            printf("Invalid flag use\n");
        }
    }

    else if (!strcmp(base_command, "help")){
        char * help = "Usage :-\n \
$ todo add 2 hello world    # Add a todo with priority 2\n \
$ todo ls                   # Show all incompleted todos sorted by priority in ascending order\n \
$ todo ls -use <file-name>  # use -use argument to use a specific todo file\n \
$ todo ls --format          # --format for formatting\n \
$ todo ls --done            # Show all completed todos sorted by priority in ascending order\n \
$ todo ls --todos           # Show all todos files\n \
$ todo del ID               # Delete a completed todo with the given ID\n \
$ todo del ID --force       # Use --force flag to delete a uncompleted todo\n \
$ todo del --all            # Delete all completed todo\n \
$ todo done ID              # Mark the incomplete todo with the given ID as complete\n \
$ todo done ID --undone     # Mark the complete todo with the given ID as incomplete\n \
$ todo report               # Statistics\n \
$ todo create FILE_NAME     # Create a new todo file\n \
$ todo use FILE_NAME        # Set the defualt TODO file\n \
$ todo help                 # Show usage\n";

        printf("%s", help);

    } else {
        printf("Invalid command\n");
    }

    return 0;
}
