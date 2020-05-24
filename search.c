#include <unistd.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>


// structure for creating textbox
struct {
    int width;
    int height;
    char * text;
    int text_len;
    Display * display;
    int screen;
    Window root;
    Window window;
    GC gc;
    XFontStruct * font;
    unsigned long black_pixel;
    unsigned long white_pixel;
} TBox;

char find[256];
int found = 0;

// File structure
struct {
    char FILENAME[256];
} Found;



// Recursive to go through all the files in this system.
void findFile(char *name)
{
    DIR *dir;
    struct dirent *entry;

    if (!(dir = opendir(name)))
        return;

    while ((entry = readdir(dir)) != NULL) {
        if(!strcmp(entry->d_name, find)){ // If the names match.
            char pathName[256]={0};
            strncpy(&pathName, name+1, strlen(name)-1);
            strncpy(&Found.FILENAME, name+1, strlen(name)-1);
            strcat(&Found.FILENAME, "/");
            strcat(&Found.FILENAME, entry->d_name);
            printGraphic(); // Pop up text box
            memset(Found.FILENAME, '\0', 256);

            found = 1;
        }
        if (entry->d_type == DT_DIR) { // If it's directory, call recursive function
            char path[1024];
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
            findFile(path);

        } 

    }
    closedir(dir);
}

// Set up textbox gui.
static void x_connect ()
{
    TBox.display = XOpenDisplay (NULL);
    if (! TBox.display) {
        fprintf (stderr, "Could not open display.\n");
        exit (1);
    }
    TBox.screen = DefaultScreen (TBox.display);
    TBox.root = RootWindow (TBox.display, TBox.screen);
    TBox.black_pixel = BlackPixel (TBox.display, TBox.screen);
    TBox.white_pixel = WhitePixel (TBox.display, TBox.screen);
}

// Set up text box
static void create_window ()
{
    TBox.width = 900;
    TBox.height = 900;
    TBox.window =
        XCreateSimpleWindow (TBox.display,
                TBox.root,
                1, // x
                1, // y
                TBox.width, 
                TBox.height,
                0, // Border width
                TBox.black_pixel, // Text color
                TBox.white_pixel); // Background color
    XSelectInput (TBox.display, TBox.window, KeyPressMask | ExposureMask);
    XMapWindow (TBox.display, TBox.window);
}


// Set up graphics context
static void set_up_gc ()
{
    TBox.screen = DefaultScreen (TBox.display);
    TBox.gc = XCreateGC (TBox.display, TBox.window, 0, 0);
    XSetBackground (TBox.display, TBox.gc, TBox.white_pixel);
    XSetForeground (TBox.display, TBox.gc, TBox.black_pixel);
}

// Set up fonts
static void set_up_font ()
{
    const char * fontname = "-*-*-*-r-*-*-20-*-*-*-*-*-*-*";
    TBox.font = XLoadQueryFont (TBox.display, fontname);
    XSetFont (TBox.display, TBox.gc, TBox.font->fid);
}


// Draw
static void Draw ()
{
    // Initialize variable
    int x;
    int y;
    int direction;
    int ascent;
    int descent;
    struct stat fstats;
    char content[100];
    char str[100];
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));
    XCharStruct overall;


    // Initial place for text
    XTextExtents (TBox.font, TBox.text, TBox.text_len,
            & direction, & ascent, & descent, & overall);
    x = (TBox.width - overall.width) / 2 + 90;
    y = TBox.height / 2 + (ascent - descent) / 2;
    XClearWindow (TBox.display, TBox.window);


    // Prints texts

    // File name
    strcat(content, "File name: ");
    strcat(content, Found.FILENAME);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));


    // Get various file information
    stat(Found.FILENAME, &fstats);



    // Device ID 
    snprintf(str, sizeof(fstats.st_dev), "%lld", fstats.st_dev);
    strcat(content, "Device: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+20, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));

    // Inode
    snprintf(str, sizeof(fstats.st_ino), "%ld", fstats.st_ino);
    strcat(content, "Inode: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+40, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));


    // Number of hard links
    snprintf(str, sizeof(fstats.st_nlink), "%ld", fstats.st_nlink);
    strcat(content, "Number of hardlinks: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+60, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));

    // File Size
    snprintf(str, sizeof(fstats.st_size), "%ld", fstats.st_size);
    strcat(content, "File size: ");
    strcat(content, str);
    strcat(content, " bytes");
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+80, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));


    // Time of last access
    snprintf(str, strlen(ctime(&fstats.st_atime)), "%s", ctime(&fstats.st_atime));
    strcat(content, "Time of last access: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+120, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));
    
    // Time of last change in file content
    snprintf(str, strlen(ctime(&fstats.st_mtime)), "%s", ctime(&fstats.st_mtime));
    strcat(content, "Time of last change in file content: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+140, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));

    // Time of last change of file status
    snprintf(str, strlen(ctime(&fstats.st_ctime)), "%s", ctime(&fstats.st_ctime));
    strcat(content, "Time of last change of file status: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+160, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));

    // Owner ID
    snprintf(str, sizeof(fstats.st_uid), "%d", fstats.st_uid);
    strcat(content, "Owner id: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+180, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));

    // Group ID
    snprintf(str, sizeof(fstats.st_gid), "%d", fstats.st_gid);
    strcat(content, "Group id: ");
    strcat(content, str);
    TBox.text = content;
    XDrawString (TBox.display, TBox.window, TBox.gc, x, y+200, TBox.text, strlen(TBox.text));
    memset(str, '\0', sizeof(str));
    memset(content, '\0', sizeof(content));


}

// Listening loop
static void event_loop ()
{
    while (1) {
        XEvent e;
        XNextEvent (TBox.display, & e);
        if (e.type == Expose) {
            Draw ();
        }
        if (e.type == KeyPress){
            break;
	}
    }
    XCloseDisplay(TBox.display);
}

// Create Text Box
void printGraphic()
{
    x_connect ();
    create_window ();
    set_up_gc ();
    set_up_font ();
    event_loop ();
}

// Main
int main(void) {
    // Get user input
    printf("Enter file name you want to search: ");
    scanf("%s", &find);

    // Start searching from the root directory
    findFile("/");
    if(!found) printf("Not Found\n");
    return 0;
}
