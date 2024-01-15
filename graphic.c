#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h> // directory business

#include <windows.h>

#define MAX_TEMPLATES 20

void print_usage() {
    fprintf(stderr, "ERROR: Incorrect number of arguments\n\n");
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, ".\\graphic.exe <arg> <opt1> <opt2>\n");
    fprintf(stderr, "arg: --get\n");
    fprintf(stderr, "\t<arg1: template>: the name (not including file extension) of a template to be found in ~/.config/inkscape-templates\n");
    fprintf(stderr, "\t<opt2: new name>: the name of the svg (and pdf_tex) files when moved to ./figures/\n");
    fprintf(stderr, "arg: --list\n");
    fprintf(stderr, "\tgenerates the list of all current templates, doesn't use additional args\n");
    fprintf(stderr, "arg: --edit\n");
    fprintf(stderr, "\t<opt1: figure>: the name of the svg (which should exist in ./figures) to open in inkscape and edit\n");
    fprintf(stderr, "arg: --edit-template\n");
    fprintf(stderr, "\t<opt1: template>: the name of the template (which should exist in ~/.config/templates) to open for editing\n");
    fprintf(stderr, "arg: --help\n");
    fprintf(stderr, "\tprints out this list\n");
    fprintf(stderr, "arg: --save\n");
    fprintf(stderr, "\t<opt1: figure>: the name of the svg to export to pdf_tex form\n");
    exit(0);
}


int main(int argc, char *argv[]) {
    // must have an argument
    if (argc < 2) {
        print_usage();
    }

    // define the directory for the templates, as well as a string to contain the path to a particular template
    char template_directory[64] = "C:\\Users\\Casey\\.config\\inkscape-templates\\";
    char template_path[64] = "";

    // define some constants, path to inkscape as well as svg file extension
    const char *svg_file_ext = "svg";
    const char *inkscape_exe_path = "C:\\Program` Files\\Inkscape\\bin\\inkscape.exe";

    // define the directory for the figures folder in the current directory
    // as well as a string to contain the path to the figure that will be edited/opened in that directory
    char current_directory[128] = "";
    char new_path[128] = "";

    // windows func to grab the currnet directory
    GetModuleFileName(NULL, current_directory, 128);
    *(strstr(current_directory, "graphic.exe")) = 0; //remove the exe file to just get directory
    strcat(current_directory, "figures\\"); // current_directory now contains the path to the figures folder


    // for the next bit, we grab the names of the templates in the template folder
    // as well as the names of the figures in the figures folder
    char template_names[MAX_TEMPLATES][32] = {0};
    char current_fig_names[MAX_TEMPLATES][32] = {0};
    int num_templates = 0;
    int num_figures = 0;
    DIR *dr;
    struct dirent *en;

    // start with the template directory
    dr = opendir(template_directory);
    if (dr == NULL) {
        fprintf(stderr, "error opening template directory\n");
        exit(1);
    }
    while ((en = readdir(dr)) != NULL) {
        if ((!strcmp(en->d_name, ".")) || (!strcmp(en->d_name, ".."))) {
            continue;
        }
        char temp_name[32];
        char *token = strtok(en->d_name, ".");
        strcpy(temp_name, token);
        
        token = strtok(NULL, ".");
        if (!strcmp(token, svg_file_ext)) {
            strcpy(template_names[num_templates++], temp_name);
        }
    }
    if (num_templates == 0) {
        fprintf(stderr, "No template files were found!");
        exit(1);
    }
    closedir(dr);
    
    // now for the current figures directory
    dr = opendir(current_directory);
    if (dr == NULL) {
        fprintf(stderr, "error opening figure directory\n");
        exit(1);
    }
    while ((en = readdir(dr)) != NULL) {
        if ((!strcmp(en->d_name, ".")) || (!strcmp(en->d_name, ".."))) {
            continue;
        }
        char temp_name[32];
        char *token = strtok(en->d_name, ".");
        strcpy(temp_name, token);
        
        token = strtok(NULL, ".");
        if (!strcmp(token, svg_file_ext)) {
            strcpy(current_fig_names[num_figures++], temp_name);
        }
    }
    closedir(dr);

    // now that we have parsed all that info, we can start checking for certain args


    // --list
    // just print out the "error" message stuff
    if (!strcmp(argv[1], "--list")) {
        for (int i=0; i<num_templates; i++) {
            printf("%s\n", template_names[i]);
        }
        exit(0);
    } else if (!strcmp(argv[1], "--help")) {
        print_usage();
    }



    // --edit?
    // check if the provided arg exists, if so open. if not, quit
    if (!strcmp(argv[1], "--edit") && argc != 3) {
        print_usage();
    } else if (!strcmp(argv[1], "--edit") && argc == 3) {
        // we need to parse the list of current_fig_names to see if the file that 
        // the user wants to edit exists or not
        int found = 0;
        for (int i=0; i<num_figures; i++) {
            if (!strcmp(argv[2], current_fig_names[i])) found = 1;
        }
        if (!found) {
            fprintf(stderr, "File not found!\n");
            exit(1);
        }

        // if reaches here, that means that the file was found; it exists, so we just open it
        sprintf(new_path, "%s%s.%s", current_directory, argv[2], svg_file_ext);

        char command[256];
        sprintf(command, "powershell.exe -command %s %s", inkscape_exe_path, new_path);
        system(command);
        // printf(command);
        exit(0);
    }

    if (!strcmp(argv[1], "--edit-template") && argc != 3) {
        print_usage();
    } else if (!strcmp(argv[1], "--edit-template") && argc == 3) {
        int found = 0;
        for (int i=0; i<num_templates; i++) {
            if (!strcmp(argv[2], template_names[i])) found = 1;
        }
        if (!found) {
            printf("Template not found!\n");
            printf("Would you like to create a new blank template to open for editing? (Y/N) ");
            char s = getchar(); getchar(); // newline

            if (s == 'n' || s == 'N') {
                exit(0);
            } else if (s == 'Y' || s == 'y') {
                sprintf(template_path, "%s%s", template_path, "blank.svg");
                sprintf(new_path, "%s%s.%s", template_directory, argv[2], svg_file_ext);

                char command[256] = "";
                sprintf(command, "powershell.exe -command Copy-Item \"%s\" -Path \"%s\"", template_path, new_path);
                // system(command);
                printf(command);
                exit(0);
            } else {
                fprintf(stderr, "Invalid option!\n");
                exit(1);
            }
        }

        // otherwise, the file was found
        sprintf(template_path, "%s%s.%s", template_directory, argv[2], svg_file_ext);

        char command[256] = "";
        sprintf(command, "powershell.exe -command %s %s", inkscape_exe_path, template_path);
        system(command);
        // printf(command);


    }




    // --get
    // copy over the template whose name is in the first argument, then rename it to the second argument
    if (!strcmp(argv[1], "--get") && argc != 4) {
        print_usage();
    } else if (!strcmp(argv[1], "--get") && argc == 4) {

        // loop through the templates directory to check if the template exists
        int found = 0;
        for (int i=0; i<num_templates; i++) {
            if (!strcmp(argv[2], template_names[i])) {
                sprintf(template_path, "%s%s.%s", template_directory, template_names[i], svg_file_ext);         
                sprintf(new_path, "%s%s.%s", current_directory, argv[3], svg_file_ext); 
                found = 1;    
            }
        }
        // this !found is slightly different
        // since its a template, we want to create a new template, still in the template directory,
        // then open that one for editing
        if (!found) {
            printf("'%s' does not exist as a template.\n", argv[2]);
            printf("Would you like to create a new template and open for editing? (Y/N)");
            char s = getchar(); getchar(); // for newline
            if (s == 'N' || s == 'n') {
                exit(0);
            } else if (s == 'Y' || s == 'y') {
                // since two names were provided, it might be unclear what user wants to name the template
                printf("What would you like to name the template? ");
                char buffer[256];
                fgets(buffer, 128, stdin);

                sprintf(template_path, "%s%s", template_directory, "blank.svg");
                sprintf(new_path, "%s%s.%s", template_path, buffer, svg_file_ext);

                sprintf(buffer, "powershell.exe -command -Copy-Item \"%s\" -Destination \"%s\"", template_path, new_path);
                system(buffer);

                sprintf(buffer, "powershell.exe -command %s %s", inkscape_exe_path, new_path);
                system(buffer);
                exit(0);
            } else {
                fprintf(stderr, "Invalid option. Exiting program...\n");
                exit(1);
            }
            exit(0);
        }
        // if we reach here, that means the template name was found; it exists

        // now that we have the template name, we can just send powershell command to copy the file over
        char command[256];
        sprintf(command, "powershell.exe -command Copy-Item \"%s\" -Destination \"%s\"", template_path, new_path);
        system(command);
        // printf(command);

        // then we just open it with inkscape
        sprintf(command, "powershell.exe -command %s %s", inkscape_exe_path, new_path);
        system(command);
        //printf(command);
    }

    // --save
    // we call the specific export function on the file passed as an arg to skip the GUI dialogs and whatnot inside inkscape
    // lets us generate the pdf_latex much faster
    // also, if the file doesn't exist, we won't prompt anything
    if (!strcmp(argv[1], "--save") && argc != 3) {
        print_usage();
    } else if (!strcmp(argv[1], "--save") && argc == 3) {
        // check if exists
        int found = 0;
        for (int i=0; i<num_figures; i++) {
            if (!strcmp(argv[2], current_fig_names[i])) found = 1;
        }
        if (!found) {
            fprintf(stderr, "File not found!\n");
            exit(1);
        }
        // THE PATH DOES NOT CONTAIN THE SVG EXTENSION HERE
        sprintf(new_path, "%s%s", current_directory, argv[2]);


        char command[256] = "";
        sprintf(command, "powershell.exe -command %s %s.svg --export-area-page --export-dpi 300 --export-type=pdf --export-latex --export-filename %s.pdf", inkscape_exe_path, new_path, new_path);
        system(command);
        // printf(command);
        exit(0);
    }


    return 0;
}