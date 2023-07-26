// Thanks to:
//  https://github.com/szhou42/osdev/blob/master/src/include/vfs.h
//  https://github.com/szhou42/osdev/blob/master/src/kernel/filesystem/vfs.c

#ifndef VFS_H_
#define VFS_H_

#include <mdos.h>
#include <mem.h>
#include <string.h>
#include <generic_tree.h>

#define PATH_SEPARATOR '/'
#define PATH_SEPARATOR_STRING "/"
#define PATH_UP  ".."
#define PATH_DOT "."
#define VFS_EXT2_MAGIC 0xeeee2222

#define O_RDONLY     0x0000
#define O_WRONLY     0x0001
#define O_RDWR       0x0002
#define O_APPEND     0x0008
#define O_CREAT      0x0200
#define O_TRUNC      0x0400
#define O_EXCL       0x0800
#define O_NOFOLLOW   0x1000
#define O_PATH       0x2000

#define FS_FILE        0x01
#define FS_DIRECTORY   0x02
#define FS_CHARDEVICE  0x04
#define FS_BLOCKDEVICE 0x08
#define FS_PIPE        0x10
#define FS_SYMLINK     0x20
#define FS_MOUNTPOINT  0x40

#define     _IFMT   0170000 /* type of file */
#define     _IFDIR  0040000 /* directory */
#define     _IFCHR  0020000 /* character special */
#define     _IFBLK  0060000 /* block special */
#define     _IFREG  0100000 /* regular */
#define     _IFLNK  0120000 /* symbolic link */
#define     _IFSOCK 0140000 /* socket */
#define     _IFIFO  0010000 /* fifo */

struct vfs_node;

typedef uint32_t (*get_file_size_callback)(struct vfs_node * node);
typedef uint32_t (*read_callback) (struct vfs_node *, uint32_t, uint32_t, char *);
typedef uint32_t (*write_callback) (struct vfs_node *, uint32_t, uint32_t, char *);
typedef void (*open_callback) (struct vfs_node*, uint32_t flags);
typedef void (*close_callback) (struct vfs_node *);
typedef struct dirent *(*readdir_callback) (struct vfs_node *, uint32_t);
typedef struct vfs_node *(*finddir_callback) (struct vfs_node *, char *name);
typedef void (*create_callback) (struct vfs_node *, char *name, uint16_t permission);
typedef void (*unlink_callback) (struct vfs_node *, char *name);
typedef void (*mkdir_callback) (struct vfs_node *, char *name, uint16_t permission);
typedef int (*ioctl_callback) (struct vfs_node *, int request, void * argp);
typedef int (*get_size_callback) (struct vfs_node *);
typedef void (*chmod_callback) (struct vfs_node *, uint32_t mode);
typedef char ** (*listdir_callback) (struct vfs_node *);

typedef struct vfs_node {
    // Baisc information about a file(note: in linux, everything is file, so the vfs_node could be used to describe a file, directory or even a device!)
    char name[256];
    void * device;
    uint32_t mask;
    uint32_t uid;
    uint32_t gid;
    uint32_t flags;
    uint32_t inode_num;
    uint32_t size;
    uint32_t fs_type;
    uint32_t open_flags;
    // Time
    uint32_t create_time;
    uint32_t access_time;
    uint32_t modified_time;

    uint32_t offset;
    unsigned nlink;
    int refcount;

    // File operations
    read_callback read;
    write_callback write;
    open_callback open;
    close_callback close;
    readdir_callback readdir;
    finddir_callback finddir;
    create_callback create;
    unlink_callback unlink;
    mkdir_callback mkdir;
    ioctl_callback ioctl;
    get_size_callback get_size;
    chmod_callback chmod;
    get_file_size_callback get_file_size;

    listdir_callback listdir;
}vfs_node_t;

struct dirent {
    char name[256];
    uint32_t inode_num;
};

typedef struct vfs_entry {
    char * name;
    vfs_node_t * file;
}vfs_entry_t;

uint32_t vfs_get_file_size(vfs_node_t * node);
uint32_t vfs_read(vfs_node_t *node, uint32_t offset, uint32_t size, char *buffer);
uint32_t vfs_write(vfs_node_t *node, uint32_t offset, uint32_t size, char *buffer);
void vfs_open(struct vfs_node *node, uint32_t flags);
void vfs_close(vfs_node_t *node);
vfs_node_t *vfs_finddir(vfs_node_t *node, char *name);
void vfs_mkdir(char *name, uint16_t permission);
void vfs_mkfile(char *name, uint16_t permission);
int vfs_create_file(char *name, uint16_t permission);
vfs_node_t *file_open(const char *file_name, uint32_t flags);
char *expand_path(char *input);
int vfs_ioctl(vfs_node_t *node, int request, void * argp);
void vfs_chmod(vfs_node_t *node, uint32_t mode);
void vfs_unlink(char * name);
int vfs_symlink(char * value, char * name);
int vfs_readlink(vfs_node_t * node, char * buf, uint32_t size);
void vfs_init();
void vfs_mount(char * path, vfs_node_t * local_root);
typedef vfs_node_t * (*vfs_mount_callback)(char * arg, char * mountpoint);
void vfs_register(char * name, vfs_mount_callback callme);
void vfs_mount_dev(char * mountpoint, vfs_node_t * node);
void print_vfstree();
void vfs_db_listdir(char * name);

gtree_t * vfs_tree;
vfs_node_t * vfs_root;

uint32_t vfs_get_file_size(vfs_node_t * node) {
    if(node && node->get_file_size) {
        return node->get_file_size(node);
    }
    return 0;
}

/*
 * This function is for debug,purpose
 * Given a dir name, it will print out all the sub-directories/files under the dir
 * */
// TODO: actually print 4
void vfs_db_listdir(char * name) {
    // Open the VFS node, call its list dir function
    vfs_node_t * n = file_open(name, 0);
    if(!n) {
        com_print(COM1, "Could not list a directory that does not exist\n");
        return;
    }
    if(!n->listdir) return;
    char ** files = n->listdir(n);
    char ** save = files;
    while(*files) {
        //qemu_printf("%s ", *files);
        kfree(*files);
        files++;
    }
    kfree(save);
    com_print(COM1, "\n");
}
/*
 * Recuersively print the vfs tree and show the device mounted on each node
 *
 * For next level, indent offset + strlen(parent path) + 1
 * /(ext2)
 *   dev(empty)
 *       hda(...)
 *       hdb
 *       hdc
 *       hdd
 *
 * */
void print_vfstree_recur(gtreenode_t * node, int parent_offset) {
    if (!node) return;
    //db_print();
    char * tmp = malloc(512);
    //db_print();
    int len = 0;
    memset(tmp, 0, 512);
    for (unsigned int i = 0; i < (int64_t) parent_offset; ++i) {
        strcat(tmp, " ");
    }
    // char * curr = tmp + strlen(tmp);
    struct vfs_entry * fnode = (struct vfs_entry *)node->value;
    if (fnode->file) {
        //com_print(curr, "%s(0x%x, %s)", fnode->name, (unsigned int)fnode->file, fnode->file->name);
    } else {
        //qemu_printf(curr, "%s(empty)", fnode->name);
    }
    //qemu_printf("%s\n", tmp);
    len = strlen(fnode->name);
    free(tmp);
    foreach(child, node->children) {
        print_vfstree_recur(child->val, parent_offset + len + 1);
    }
}
/*
 * Wrapper for vfs_db_print_recur
 * */
void print_vfstree() {
    print_vfstree_recur(vfs_tree->root, 0);
}

/*
 * Wrapper for physical filesystem read
 * */

unsigned int vfs_read(vfs_node_t *node, unsigned int offset, unsigned int size, char *buffer) {
    if (node && node->read) {
        unsigned int ret = node->read(node, offset, size, buffer);
        return ret;
    }
    return -1;
}

/*
 * Wrapper for physical filesystem write
 * */
unsigned int vfs_write(vfs_node_t *node, unsigned int offset, unsigned int size, char *buffer) {
    if (node && node->write) {
        unsigned int ret = node->write(node, offset, size, buffer);
        return ret;
    }
    return -1;
}


/*
 * Wrapper for physical filesystem open
 * */
void vfs_open(struct vfs_node *node, unsigned int flags) {
    if(!node) return;
    if(node->refcount >= 0) node->refcount++;
    node->open(node, flags);
}

/*
 * Wrapper for physical filesystem close
 * */
void vfs_close(vfs_node_t *node) {
    if(!node || node == vfs_root || node->refcount == -1) return;
    node->refcount--;
    if(node->refcount == 0)
        node->close(node); // Should I free the node ? No, the caller should do it.
}

/*
 * Wrapper for physical filesystem chmod
 * */
void vfs_chmod(vfs_node_t *node, uint32_t mode) {
    if (node->chmod) return node->chmod(node, mode);
}

/*
 * Wrapper for physical filesystem readdir
 * */
struct dirent *vfs_readdir(vfs_node_t *node, unsigned int index) {
    if(node && (node->flags & FS_DIRECTORY) && node->readdir)
        return node->readdir(node, index);
    return NULL;
}

/*
 * Wrapper for physical filesystem finddir
 * */
vfs_node_t *vfs_finddir(vfs_node_t *node, char *name) {
    if(node && (node->flags & FS_DIRECTORY) && node->finddir)
        return node->finddir(node, name);
    return NULL;
}

/*
 * Wrapper for physical filesystem ioctl
 * */
int vfs_ioctl(vfs_node_t *node, int request, void * argp) {
    if(!node) return -1;
    if(node->ioctl) return node->ioctl(node, request, argp);
    return -1;
}

/*
 * Wrapper for physical filesystem mkdir
 *
 * */
void vfs_mkdir(char *name, unsigned short permission) {
    // First, extract the parent directory and the directory to be made
    int i = strlen(name);
    char * dirname = strdup(name);
    char * save_dirname = dirname;
    char * parent_path = "/";
    while(i >= 0) {
        if(dirname[i] == '/') {
            if(i != 0) {
                dirname[i] = '\0';
                parent_path = dirname;
            }
            dirname = &dirname[i+1];
            break;
        }
        i--;
    }

    // Second, file_open
    vfs_node_t * parent_node = file_open(parent_path, 0);
    if(!parent_node) {
        kfree(save_dirname);
    }

    // Third, call mkdir
    if(parent_node->mkdir)
        parent_node->mkdir(parent_node, dirname, permission);
    kfree(save_dirname);

    vfs_close(parent_node);
}

/*
 * Wrapper for physical filesystem create file under a directory
 *
 * */

int vfs_create_file(char *name, unsigned short permission) {
    // First, extract the parent directory and the directory to be made
    int i = strlen(name);
    char * dirname = strdup(name);
    char * save_dirname = dirname;
    char * parent_path = "/";
    while(i >= 0) {
        if(dirname[i] == '/') {
            if(i != 0) {
                dirname[i] = '\0';
                parent_path = dirname;
            }
            dirname = &dirname[i+1];
            break;
        }
        i--;
    }

    // Second, file_open
    vfs_node_t * parent_node = file_open(parent_path, 0);
    if(!parent_node) {
        kfree(save_dirname);
        return -1;
    }
    if(parent_node->create)
        parent_node->create(parent_node, dirname, permission);
    kfree(save_dirname);
    vfs_close(parent_node);
    return 0;


}
/*
 * Wrapper for physical filesystem unlink (delete) file under a directory
 *
 * */

void vfs_unlink(char * name) {
    // First, extract the parent directory and the directory to be made
    int i = strlen(name);
    char * dirname = strdup(name);
    char * save_dirname = dirname;
    char * parent_path = "/";
    while(i >= 0) {
        if(dirname[i] == '/') {
            if(i != 0) {
                dirname[i] = '\0';
                parent_path = dirname;
            }
            dirname = &dirname[i+1];
            break;
        }
        i--;
    }

    // Second, file_open
    vfs_node_t * parent_node = file_open(parent_path, 0);
    if(!parent_node) {
        kfree(save_dirname);
    }
    if(parent_node->unlink)
        parent_node->unlink(parent_node, dirname);
    kfree(save_dirname);
    vfs_close(parent_node);
}


/*
 * Parse the path and replace special meaning dir name such as "." and ".."
    For example, /abc/def/.. is converted to /abc

stack:
 .. def abc
for normal token, just pop it out
when .. is seen, pop it and pop one more time
*/
char *expand_path(char *input) {
    // First, push all of them onto a stack
    list_t * input_list = str_split(input, "/", NULL);
    char * ret = list2str(input_list, "/");
    return ret;
}

/*
 * Helper function for file_open
 * Given a filename, return the vfs_node_t of the path on which it's mounted
 * For example, in our case /home/szhou42 is mounted on the vfs_node_t with path "/" (ext2 is mounted for root dir)
 *
 * */
vfs_node_t * get_mountpoint_recur(char ** path, gtreenode_t * subroot) {
    int found = 0;
    char * curr_token = strsep(path, "/");
    // Basecase, not getting any more tokens, stop and return the last one
    if(curr_token == NULL || !strcmp(curr_token, "")) {
        struct vfs_entry * ent = (struct vfs_entry*)subroot->value;
        return ent->file;
    }
    // Find if subroot's children contain any that matches the current token
    foreach(child, subroot->children) {
        gtreenode_t * tchild = (gtreenode_t*)child->val;
        struct vfs_entry * ent = (struct vfs_entry*)(tchild->value);
        if(strcmp(ent->name, curr_token) == 0) {
            found = 1;
            subroot = tchild;
            break;
        }
    }

    if(!found) {
        // This token is not found, make path point to this token so that file_open knows from where to start searching in the physical filesystem
        // In another words, for a path like "/home/szhou42", the vfs tree is only aware of the root path "/", because it's the only thing mounted here
        // For the rest of the path "home/szhou42", file_open have to find them in the physical filesystem(such as ext2)
        *path = curr_token;
        return ((struct vfs_entry*)(subroot->value))->file;
    }
    // Recursion !
    return get_mountpoint_recur(path, subroot);
}
/*
 * Wrapper function for get_mountpoint_recur
 *
 * */
vfs_node_t * get_mountpoint(char ** path) {
    // Adjust input, delete trailing slash
    if(strlen(*path) > 1 && (*path)[strlen(*path) - 1] == '/')
        *(path)[strlen(*path) - 1] = '\0';
    if(!*path || *(path)[0]!= '/') return NULL;
    if(strlen(*path) == 1) {
         // root, clear the path
        *path = '\0';
        struct vfs_entry * ent = (struct vfs_entry*)vfs_tree->root->value;
        return ent->file;

    }
    (*path)++;
    return get_mountpoint_recur(path, vfs_tree->root);
}
/*
 * Given filename, return a vfs_node(Then you can do reading/writing on the file! Pretty much like fopen)
 * */
vfs_node_t *file_open(const char * file_name, unsigned int flags) {
    /* First, find the mountpoint of the file(i.e find which filesystem the file belongs to so that vfs can call the right functions for accessing directory/files)
     Since the vfs tree doesn't store directory tree within a physical filesystem(i.e when ext2 is mounted on /abc, the vfs tree doesn't maintain any sub-directories under /abc),
     we will need to traverse the tree using callback provided by physical filesystem(ext2 for example)
     When it gets to  directory containing the file, simply invoke the callback open provided by the physical filesystem
    */
    char * curr_token = NULL;
    char * filename = strdup(file_name);
    char * free_filename = filename;
    char * save = strdup(filename);
    char * original_filename = filename;
    char * new_start = NULL;
    vfs_node_t * nextnode = NULL;
    vfs_node_t * startpoint = get_mountpoint(&filename);
    if(!startpoint) return NULL;
    if(filename)
        new_start = strstr(save + (filename - original_filename), filename);
    while( filename != NULL  && ((curr_token = strsep(&new_start, "/")) != NULL)) {
        nextnode = vfs_finddir(startpoint, curr_token);
        if(!nextnode) return NULL;
        startpoint = nextnode;
    }
    if(!nextnode)
        nextnode = startpoint;
    vfs_open(nextnode, flags);
    kfree(save);
    kfree(free_filename);
    return nextnode;
}

/*
 * Set up a filesystem tree, for which device and filesystem can be mounted on
 * Set up hashmap, filesysyems can register its initialization callback to the vfs
 * */
void vfs_init() {
    vfs_tree = tree_create();
    struct vfs_entry * root = malloc(sizeof(struct vfs_entry));
    root->name = strdup("root");
    root->file = NULL;
    tree_insert(vfs_tree, NULL, root);
}

/*
 * Mounting a device is no different from inserting a plain node onto the tree
 * */
void vfs_mount_dev(char * mountpoint, vfs_node_t * node) {
    vfs_mount(mountpoint, node);
}

/*
 * Helper function for vfs_mount
 * This is simply inserting a node
 * */
void vfs_mount_recur(char * path, gtreenode_t * subroot, vfs_node_t * fs_obj) {
    int found = 0;
    char * curr_token = strsep(&path, "/");

    if(curr_token == NULL || !strcmp(curr_token, "")) {
        // return the subroot, it's where u should mount!
        struct vfs_entry * ent = (struct vfs_entry*)subroot->value;
        if(ent->file) {
            com_print(COM1, "The path is already mounted, plz unmount before mounting again\n");
            return;
        }
        if(!strcmp(ent->name, "/")) vfs_root = fs_obj; // Keep a shortcut for root node
        ent->file = fs_obj;
        return;
    }

    foreach(child, subroot->children) {
        gtreenode_t * tchild = (gtreenode_t*)child->val;
        struct vfs_entry * ent = (struct vfs_entry*)(tchild->value);
        if(strcmp(ent->name, curr_token) == 0) {
            found = 1;
            subroot = tchild;
        }
    }

    if(!found) {
         // Insert the node by myself
         struct vfs_entry * ent = kcalloc(sizeof(struct vfs_entry), 1);
         ent->name = strdup(curr_token);
         subroot = tree_insert(vfs_tree, subroot, ent);
    }
    // Recursion!
    vfs_mount_recur(path, subroot, fs_obj);
}
/* Wrapper function for vfs_mount_recur
 * Simply insert a node to the tree
 * */
void vfs_mount(char * path, vfs_node_t * fs_obj) {
    fs_obj->refcount = -1;
    fs_obj->fs_type= 0;
    if(path[0] == '/' && strlen(path) == 1) {
        struct vfs_entry * ent = (struct vfs_entry*)vfs_tree->root->value;
        if(ent->file) {
            com_print(COM1, "The path is already mounted, plz unmount before mounting again\n");
            return;
        }
        vfs_root = fs_obj; // Keep a shortcut for root node
        ent->file = fs_obj;
        return;

    }
    vfs_mount_recur(path + 1, vfs_tree->root, fs_obj);
}

#endif