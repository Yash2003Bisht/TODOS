#include "todos.c"

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
            size = argc-3;
            char todo[size][1000], priority = argv[2][0];

            // copy the todo to a new char array
            for (int i=0; i < size; i++)
                strcpy(todo[i], argv[i+3]);        

            add_todo(priority, size, todo);
            printf("Todo added\n");
        }
    }

    else if (!strcmp(base_command, "done")){
        if (argc <= 2){
            printf("Todo id is required\n");
        } else{
            todo_id = atoi(argv[2]);
            if (argc == 4)
                mark_todo(todo_id, all_todos, argv[3]);
            else
                mark_todo(todo_id, all_todos, "--done");
        }
    }

    else if (!strcmp(base_command, "del")){
        if (argc <= 2){
            printf("Todo id is required\n");
        } else if (!strcmp(argv[2], "--all")){
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
            int force_del;

            // check if "--force" delete flag is used
            if (argc >= 4){
                if (!strcmp(argv[3], "--force"))
                    force_del = 1;
                else
                    force_del = 0;
            }

            del_todo(todo_id, force_del, all_todos);
        }
    }

    else if (!strcmp(base_command, "report")){
        generate_report(all_todos);
    }

    else if (!strcmp(base_command, "ls")){
        if (argc <= 2){
            list_pending_todos(all_todos, 0);
        } else {
            // Get all flags used
            int done = 0, format = 0, todos = 0;

            for (int i=2; i<argc; i++){
                if (!strcmp(argv[i], "--todos"))
                    todos = 1;
                else if (!strcmp(argv[i], "--format"))
                    format = 1;
                else if (!strcmp(argv[i], "--done"))
                    done = 1;
            }

            if (todos)
                list_all_todos_files();
            else if (done)
                list_completed_todos(all_todos, format);
            else if (format)
                list_pending_todos(all_todos, format);
            // special case for '-use' parameter
            // check if the '-use' parameter is used than it's not an invalid flag, just list out all the pending todos.
            else if (use_file)
                list_pending_todos(all_todos, 0);
            else
                printf("Invalid flag use\n");
        }
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
$ todo ls --format          # --format for formatting\n \
$ todo ls --done            # Show all completed todos sorted by priority in ascending order\n \
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
