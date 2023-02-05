import sys

Help = """
$ python3 task.py help
Usage :-
$ ./task add 2 hello world             # Add a new item with priority 2 and text "hello world" to the list
$ python3 task.py ls                   # Show incomplete priority list items sorted by priority in ascending order
$ python3 task.py del INDEX            # Delete the incomplete item with the given index
$ python3 task.py done INDEX           # Mark the incomplete item with the given index as complete
$ python3 task.py help                 # Show usage
$ python3 task.py report               # Statistics
"""

def file_manager(deletor = None, done = None, ls = None):
    """manage operation like delete an item, mark as done, set the priority etc"""

    with open("task.txt") as file:
        task_file = file.readlines()
        
        """This is additional, if there is no data in file programme just print File is empty and quit"""
        # if len(task_file) == 0:
            # print("File is empty")
            # quit()

        try:
            if deletor != None:
                del task_file[int(deletor)-1]
                print(f"Deleted item with index {deletor}")
            
            if done != False and ("(done)" not in task_file[int(done)-1]):
                task_file[int(done)-1] = f"0. (done) {task_file[int(done)-1][3:]}"
                print("Marked item as done.")
            
        except IndexError:
            if deletor != None:
                print(f"Error: item with index {deletor} does not exist. Nothing deleted.")
            elif done != None:
                print(f"Error: no incomplete item with index {done} exists.")
        
        except Exception:
            pass
        

        task_file.sort(key=lambda x: list(map(int, x[-3:-2])))

        file_write = open("task.txt", "w")
        counter = 1
        for user_data in task_file:
            user_data = user_data.replace("\n", "")
            if ls != None:
                print(f"{counter}.{user_data[2:]}")
            file_write.write(f"{counter}.{user_data[2:]}\n")
            counter += 1
        file_write.close()


def file_write(priority, data):
    """Add a new item"""
    with open("task.txt", "a") as file:
        file.write(f"0. {data} [{str(priority)}]\n")
    print(f"Added task: \"{data}\" with priority {priority}")
    file_manager()

def statistics():
    """Generate a report"""
    file_manager()
    pending_tasks = []
    completed_tasks = []

    with open("task.txt") as stats:
        for stat in stats.readlines():
            if "(done)" in stat:
                completed_tasks.append(stat.replace(f"{stat[0:2]} (done) ","")[:-5])
            else:
                pending_tasks.append(stat[3:].replace("\n", ""))

    print(f"Pending : {len(pending_tasks)}")
    for pending in range(1, len(pending_tasks)+1):
        print(f"{pending}. {pending_tasks[pending-1]}")
    
    print()
    
    print(f"Completed : {len(completed_tasks)}")
    for completed in range(1, len(completed_tasks)+1):
        print(f"{completed}. {completed_tasks[completed-1]}")


def task_manager(commands):
    """function to manage the tasks"""

    command = commands[1]

    if command == "help":
        print(Help)

    elif command == "ls":
        file_manager(ls="ls")

    elif command == "add":
        file_write(commands[2], commands[3])

    elif command == "del":
        file_manager(deletor=commands[2])
    
    elif command == "done":
        file_manager(done=commands[2])
    
    elif command == "report":
        statistics()

def main():
    """main function"""
    query = sys.argv
    task_manager(query)

main()
