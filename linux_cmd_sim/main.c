#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir {
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

void touch (Dir* parent, char* name) {
	File* current_files = parent->head_children_files;
	File* previous;
	while(current_files != NULL) {
		if(strcmp(current_files->name,name) == 0) {
			printf("File already exists\n");
			return;
		}
		previous = current_files;
		current_files = current_files->next;
	}
	Dir* current_dirs = parent->head_children_dirs;
	while(current_dirs != NULL) {
		if(strcmp(current_dirs->name,name) == 0) {
			printf("File already exists\n");
			return;
		}
		current_dirs = current_dirs->next;
	}

	File* current = (File *)malloc(sizeof(File));
	current->name = strdup(name);
	current->parent = parent;
	current->next = NULL;
	if(parent->head_children_files == NULL)
		parent->head_children_files = current;
	else {
		previous->next = current;
	}
}

void mkdir (Dir* parent, char* name) {
	File* current_files = parent->head_children_files;
	while(current_files != NULL) {
		if(strcmp(current_files->name,name) == 0) {
			printf("Directory already exists\n");
			return;
		}
		current_files = current_files->next;
	}
	Dir* current_dirs = parent->head_children_dirs;
	Dir* previous;
	while(current_dirs != NULL) {
		if(strcmp(current_dirs->name,name) == 0) {
			printf("Directory already exists\n");
			return;
		}
		previous = current_dirs;
		current_dirs = current_dirs->next;
	}

	Dir* current = (Dir *)malloc(sizeof(Dir));
	current->name = strdup(name);
	current->parent = parent;
	current->head_children_dirs = NULL;
	current->head_children_files = NULL;
	current->next = NULL;
	if(parent->head_children_dirs == NULL)
		parent->head_children_dirs = current;
	else {
		previous->next = current;
	}
}

void ls (Dir* parent) {
	if(parent->head_children_dirs == NULL 
		&& parent->head_children_files == NULL)
		printf("\n");
	else {
		Dir *current_dirs = parent->head_children_dirs;
		while(current_dirs != NULL) {
			printf("%s\n",current_dirs->name);
			current_dirs = current_dirs->next;
		}
		File *current_files = parent->head_children_files;
		while(current_files != NULL) {
			printf("%s\n",current_files->name);
			current_files = current_files->next;
		}
	}
}

void rm (Dir* parent, char* name) {
	File* current_files = parent->head_children_files;
	File* previous = current_files;
	int ok = 0;
	while(current_files != NULL) {
		if(strcmp(current_files->name,name) == 0) {
			ok = 1;
			break;
		}
		previous = current_files;
		current_files = current_files->next;
	}
	if(!ok) {
		printf("Could not find the file\n");
		return;
	}
	else {
		if(previous == current_files) {
			parent->head_children_files = previous->next;
			free(previous->name);
			free(previous);
		}
		else {
			previous->next = current_files->next;
			free(current_files->name);
			free(current_files);
		}
	}
}

void rmdir (Dir* parent, char* name) {
	if(name != NULL) {
		Dir* current_dirs = parent->head_children_dirs;
		Dir* previous = current_dirs;
		int ok = 0;
		while(current_dirs != NULL) {
			if(strcmp(current_dirs->name,name) == 0) {
				ok = 1;
				break;
			}
			previous = current_dirs;
			current_dirs = current_dirs->next;
		}

		if(!ok) {
			printf("Could not find the dir\n");
			return;
		}
		else {
			if(current_dirs->head_children_dirs != NULL)
				rmdir(current_dirs->head_children_dirs,NULL);
			while(current_dirs->head_children_files != NULL) {
				rm(current_dirs,current_dirs->head_children_files->name);
			}
			if(previous == current_dirs) {
				parent->head_children_dirs = previous->next;
				free(previous->name);
				free(previous);
				return;
			}
			else {
				previous->next = current_dirs->next;
				free(current_dirs->name);
				free(current_dirs);
				return;
			}
		}
	}
	else {
		if(parent->head_children_dirs == NULL  && 
			parent->next == NULL) {
			while(parent->head_children_files != NULL)
				rm(parent,parent->head_children_files->name);
			Dir* elim = parent->parent->head_children_dirs;
			Dir* elim_prev = elim;
			while(elim != parent) {
				elim_prev = elim;
				elim = elim->next;
			}
			free(parent->name);
			free(parent);
			if(elim_prev != elim)
			elim_prev->next = NULL;
			return;
		}

		if(parent->next != NULL)
			rmdir(parent->next , NULL);
		
		if(parent->head_children_dirs != NULL)
			rmdir(parent->head_children_dirs , NULL);

		while(parent->head_children_files != NULL)
			rm(parent , parent->head_children_files->name);
		Dir* elim = parent->parent->head_children_dirs;
		Dir* elim_prev = elim;
		while(elim != parent) {
			elim_prev = elim;
			elim = elim->next;
		}
		free(parent->name);
		free(parent);
		if(elim_prev != elim)
		elim_prev->next = NULL;
	}
}

void cd(Dir** target, char *name) {
	if(!strcmp(name,"..")) {
		if((*target)->parent != NULL)
			*(target) = (*target)->parent;
	}
	else {
		Dir* current_dirs = (*target)->head_children_dirs;
		int ok = 0;
		while(current_dirs != NULL) {
			if(strcmp(current_dirs->name,name) == 0) {
				ok = 1;
				break;
			}
			current_dirs = current_dirs->next;
		}
		if(!ok)
			printf("No directories found!\n");
		else
			*(target) = current_dirs;
	}
}

char *pwd (Dir* target) {
	char* sir;
	if(target->parent == NULL) {
		sir = malloc(MAX_INPUT_LINE_SIZE);
		strcpy(sir , "/");
		strcat(sir , target->name);
		return sir;
	}
	else
		sir = pwd(target->parent);
	
	strcat(sir , "/");
	strcat(sir , target->name);
	*(sir +strlen(sir)) = '\0';
	return sir;
}

void stop (Dir* target) {		
	if(target->head_children_dirs != NULL)
		rmdir(target->head_children_dirs,NULL);
	while(target->head_children_files != NULL) {
		free(target->head_children_files->name);
		File *aux = target->head_children_files;
		target->head_children_files = target->head_children_files->next;
		free(aux);
	}
	free(target->name);
	free(target);
}

void tree (Dir* target, int level) {
	File* current_files;
	int indent = level*4;
	if(!level)
		printf("%s\n" , target->name);
	else if(level > 0)
		printf("%*s%s\n" , indent , " " , target->name);
	if(target->head_children_dirs == NULL  && 
		target->next == NULL) {
		current_files = target->head_children_files; 
		while(current_files != NULL) {
			printf("%*s%s\n" , indent+4 , " " , current_files->name);
			current_files = current_files->next;
		}
		return;
	}

	if(target->head_children_dirs != NULL )
		tree(target->head_children_dirs , level+1);

	current_files = target->head_children_files; 
	while(current_files != NULL) {
		if(level == -1)
			printf("%*s\n" , indent+4 , current_files->name);
		else 
			printf("%*s%s\n" , indent+4 , " " , current_files->name);
		current_files = current_files->next;
	}
	
	if(target->next != NULL)
		tree(target->next , level);
}

void mv(Dir* parent, char *oldname, char *newname) {
	int ok1 = 0;
	int ok2 = 0;
	File* current_files = parent->head_children_files;
	File* previous_files = current_files;
	File* current_found_file = current_files;
	File* previous_found_file = previous_files;
	while(current_files != NULL) {
		if(strcmp(current_files->name,oldname) == 0) {
			ok1 = 1;
			current_found_file = current_files;
			previous_found_file = previous_files;
		}
		if(strcmp(current_files->name,newname) == 0) {
			ok2 = 1;
			break;
		}
		previous_files = current_files;
		current_files = current_files->next;
	}
	Dir* current_dirs = parent->head_children_dirs;
	Dir* previous_dirs = current_dirs;
	Dir* current_found_dir = current_dirs;
	Dir* previous_found_dir = previous_dirs;
	while(current_dirs != NULL) {
		if(strcmp(current_dirs->name,oldname) == 0) {
			ok1 = 1;
			current_found_dir = current_dirs;
			previous_found_dir = previous_dirs;	
		}
		if(strcmp(current_dirs->name,newname) == 0) {
			ok2 = 1;
			break;
		}
		previous_dirs = current_dirs;
		current_dirs = current_dirs->next;
	}
	if(!ok1)
		printf("File/Director not found\n");
	else {
		if(ok2) {
			printf("File/Director already exists\n");
			return;
		}
		if(current_found_file != NULL) {
			if(previous_found_file == current_found_file)
				parent->head_children_files = current_found_file->next;
			else
				previous_found_file->next = current_found_file->next;
			File* aux_files = parent->head_children_files;
			if(aux_files != NULL) {
				while(aux_files->next != NULL)
					aux_files = aux_files->next;
				aux_files->next = current_found_file;
			}
			else
				parent->head_children_files = current_found_file;
			free(current_found_file->name);
			current_found_file->name = strdup(newname);
			current_found_file->next = NULL;
		}
		else if(current_found_dir != NULL) {
			if(current_found_dir == current_found_dir)
				parent->head_children_dirs = current_found_dir->next;
			else
				current_found_dir->next = current_found_dir->next;
			Dir* aux_dirs = parent->head_children_dirs;
			if(aux_dirs != NULL) {
				while(aux_dirs->next != NULL)
					aux_dirs = aux_dirs->next;
				aux_dirs->next = current_found_dir;
			}
			else
				parent->head_children_dirs = current_found_dir;
			free(current_found_dir->name);
			current_found_dir->name = strdup(newname);
			current_found_dir->next = NULL;
		}
	}
}

int main () {
	int done = 0;
	Dir *start =(Dir *)malloc(sizeof(Dir));
	start->name = strdup("home");
	start->parent = NULL;
	start->head_children_dirs = NULL;
	start->head_children_files = NULL;
	start->next = NULL;
	Dir *home = start;
	do {
	/*Summary:
		Reads from stdin a string and breaks it down into command and in
		case it needs into a name.*/
	
		char *string = malloc(MAX_INPUT_LINE_SIZE);
		if(fgets(string,MAX_INPUT_LINE_SIZE,stdin) == NULL)
			printf("No input.\n");
		else {
				char *command = NULL;
				char *parameter = NULL;
				char *ind = strchr(string,' ');
				int len1, len2;
				if(ind != NULL) {
					len1 = ind - string;
					len2 = strlen(ind) - 2;
					command = malloc(len1+1);
					parameter = malloc(len2+1);
					strncpy(command, string, len1);
					*(command+len1)='\0';
					strncpy(parameter, ind + 1, len2);
					*(parameter+len2)='\0';
				}
				else {
					command = strdup(string);
					*(command + strlen(command)-1) = '\0';
				}
				if(!strcmp(command,"touch"))
					touch(start,parameter);
				else if (!strcmp(command,"mkdir"))
					mkdir(start,parameter);
				else if (!strcmp(command,"ls"))
					ls(start);
				else if (!strcmp(command,"rm"))
					rm(start,parameter);
				else if (!strcmp(command,"rmdir"))
					rmdir(start,parameter);
				else if (!strcmp(command,"cd"))
					cd(&start,parameter);
				else if (!strcmp(command,"pwd")) {
					char *aux = pwd(start);
					printf("%s\n",aux);
					free(aux);
				}
				else if (!strcmp(command,"stop")) {
					stop(home);
					done = 1;
				}
				else if (!strcmp(command,"tree"))
					tree(start,-1);
				else if (!strcmp(command,"mv")) {
					ind = strchr(parameter,' ');
					len1 = ind - parameter;
					len2 = strlen(ind) - 1;
					char *parameter1 = malloc(len1 + 1);
					char *parameter2 = malloc(len2 + 1);
					strncpy(parameter1, parameter, len1);
					*(parameter1 + len1) = '\0';
					strncpy(parameter2, ind + 1, len2);
					*(parameter2 + len2) = '\0';
					mv(start,parameter1,parameter2);
					free(parameter1);
					free(parameter2);
				}
				free(command);
				if(parameter != NULL)
					free(parameter);
				free(string);						
			}	
	} while (!done);
	return 0;
}