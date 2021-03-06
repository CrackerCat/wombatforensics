#ifndef SQFUSE_H
#define SQFUSE_H

/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPLv2.
  See the file COPYING.
*/

/** @file
 *
 * minimal example filesystem using high-level API
 *
 * Compile with:
 *
 *     gcc -Wall hello.c `pkg-config fuse3 --cflags --libs` -o hello
 *
 * ## Source code ##
 * \include hello.c
 *
 * Modified by Pasquale J. Rinaldi, Jr. to be used for aff fuse mounting
 * as a function.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>

#define FUSE_USE_VERSION 35

#include <fuse3/fuse.h>
#include <fuse3/fuse_lowlevel.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <assert.h>
#include <pthread.h>
#include <sys/fsuid.h>
#include <paths.h>
extern "C" {
//#include "unsquashfs.h"
#include "squash.h"
}
#include <iostream>  
#include <fstream>  
      
uint8_t* ReadFileBytes(const char *name)  
{
    qDebug() << "start read file bytes";
    FILE* pfile;
    long lsize;
    uint8_t* retbuf;
    size_t result;
    pfile = fopen(name, "rb");
    fseek(pfile, 0, SEEK_END);
    lsize = ftell(pfile);
    qDebug() << "lsize:" << lsize;
    rewind(pfile);
    retbuf = (uint8_t*)malloc(sizeof(uint8_t)*lsize);
    result = fread(retbuf, 1, lsize, pfile);
    fclose(pfile);
    qDebug() << "end read file bytes";
    return retbuf;
    /*
     * fread example: read an entire file 
#include <stdio.h>
#include <stdlib.h>

int main () {
  FILE * pFile;
  long lSize;
  char * buffer;
  size_t result;

  pFile = fopen ( "myfile.bin" , "rb" );
  if (pFile==NULL) {fputs ("File error",stderr); exit (1);}

  // obtain file size:
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);

  // allocate memory to contain the whole file:
  buffer = (char*) malloc (sizeof(char)*lSize);
  if (buffer == NULL) {fputs ("Memory error",stderr); exit (2);}

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,pFile);
  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}

  // the whole file is now loaded in the memory buffer.

  // terminate
  fclose (pFile);
  free (buffer);
  return 0;
     */ 
    /*
    std::ifstream fl(name);  
    fl.seekg( 0, std::ios::end );  
    size_t len = fl.tellg();  
    uint8_t *ret = new uint8_t[len];  
    fl.seekg(0, std::ios::beg);   
    fl.read((char*)ret, len);  
    fl.close();  
    return ret;  
    */
}  

//int sqvfd = 0;
/*
typedef struct squishfs squishfs;
struct squishfs {
    sqfs fs;
    sqfs_inode root;
};

squishfs* squish;

static sqfs_err sqfuse_lookup(sqfs** fs, sqfs_inode* inode, const char* path)
{
    qDebug() << "sqfuse_lookup path:" << QString::fromStdString(std::string(path));
    bool found;
    squishfs* squisher = (squishfs*)(fuse_get_context()->private_data);
    *fs = &squisher->fs;
    if(inode)
    {
        qDebug() << "inode dir start block:" << inode->xtra.dir.start_block;
	//*inode = squisher->root; // copy
    }
    if(path)
    {
	sqfs_err err = sqfs_lookup_path(*fs, inode, path, &found);
	if(err)
	    return err;
	if(!found)
	    return SQFS_ERR;
    }
    return SQFS_OK;
};

sqfs_err sqfs_stat(sqfs *fs, sqfs_inode *inode, struct stat *st) {
	sqfs_err err = SQFS_OK;
	uid_t id;
	
	memset(st, 0, sizeof(*st));
	st->st_mode = inode->base.mode;
	st->st_nlink = inode->nlink;
	st->st_mtime = st->st_ctime = st->st_atime = inode->base.mtime;
	
	if (S_ISREG(st->st_mode)) {
		/* FIXME: do symlinks, dirs, etc have a size? */
/*		st->st_size = inode->xtra.reg.file_size;
		st->st_blocks = st->st_size / 512;
	} else if (S_ISBLK(st->st_mode) || S_ISCHR(st->st_mode)) {
		st->st_rdev = sqfs_makedev(inode->xtra.dev.major,
			inode->xtra.dev.minor);
	} else if (S_ISLNK(st->st_mode)) {
		st->st_size = inode->xtra.symlink_size;
	}
	
	st->st_blksize = fs->sb.block_size; /* seriously? */
	
/*	err = sqfs_id_get(fs, inode->base.uid, &id);
	//if (err)
	//	return err;
	st->st_uid = id;
	err = sqfs_id_get(fs, inode->base.guid, &id);
	st->st_gid = id;
	//if (err)
	//	return err;
	
	return SQFS_OK;
};
dev_t sqfs_makedev(int maj, int min)
{
    return makedev(maj, min);
};

#ifndef ENOATTR
	 #define ENOATTR ENODATA
#endif

int sqfs_enoattr() {
	return ENOATTR;
};

int sqfs_listxattr(sqfs *fs, sqfs_inode *inode, char *buf, size_t *size) {
	sqfs_xattr x;
	size_t count = 0;
	
	if (sqfs_xattr_open(fs, inode, &x))
        {
            qDebug() << "listxattr error";
		return -EIO;
        }
	
	while (x.remain) {
		size_t n;
		if (sqfs_xattr_read(&x))
                {
                    qDebug() << "listxattr error 2";
                    return EIO;
                }
		n = sqfs_xattr_name_size(&x);
		count += n + 1;
		
		if (buf) {
			if (count > *size)
				return ERANGE;
			if (sqfs_xattr_name(&x, buf, true))
                        {
                            qDebug() << "listxattr error 3";
				return EIO;
                        }
			buf += n;
			*buf++ = '\0';
		}
	}
	*size = count;
	return 0;
};

int sqfs_statfs(sqfs *sq, struct statvfs *st) {
	struct squashfs_super_block *sb = &sq->sb;

	st->f_bsize = sb->block_size;
	st->f_frsize = sb->block_size;
	st->f_blocks = ((sb->bytes_used - 1) >> sb->block_log) + 1;
	st->f_bfree = 0;
	st->f_bavail = 0;
	st->f_files = sb->inodes;
	st->f_ffree = 0;
	st->f_favail = 0;
	st->f_namemax = SQUASHFS_NAME_LEN;

	return 0;
};
*/
//sqfs squish;
//sqfs_inode sqroot;

//static int sqvfd = 0;
//static sqfs* squish = NULL;
//libewf_handle_t* ewfhandle = NULL;
//libewf_error_t* ewferror = NULL;
static char* sqrawpath = NULL;
static off_t sqrawsize = 15;
static const char* sqrawext = ".dd";

/*
#define XCALLOC(type, num) ((type *) xcalloc ((num), sizeof(type)))
#define XMALLOC(type, num) ((type *) xmalloc ((num) * sizeof(type)))
#define XFREE(stale) do { if(stale) { free ((void*) stale); stale = 0; } } while (0)
*/
//static AFFILE* afimage = NULL;

/*
static void* xmalloc(size_t num)
{
    void* alloc = malloc(num);
    if(!alloc)
    {
	perror("Memory Exhausted");
	exit(EXIT_FAILURE);
    }
    return alloc;
};

static void* xcalloc(size_t num, size_t size)
{
    void* alloc = xmalloc(num*size);
    memset(alloc, 0, num*size);
    return alloc;
};

static char* xstrdup(char* string)
{
    return strcpy((char*)xmalloc(strlen(string) + 1), string);
};
*/

static void* sqfuse_init(struct fuse_conn_info* conn, struct fuse_config* cfg)
{
    qDebug() << "sqfuse_init() start";
    //(void) conn;
    //cfg->kernel_cache = 1;
    return fuse_get_context()->private_data;
    //return NULL;
};

static int sqfuse_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi)
{
    int res = 0;
    qDebug() << "sqfuse_getattr() run";
    qDebug() << "path:" << QString::fromStdString(std::string(path));
    //sqfs* fs;
    //sqfs_inode inode;
    if(strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else if(strcmp(path, sqrawpath) == 0)
    {
        stbuf->st_mode = S_IFREG | 0444;
        stbuf->st_nlink = 1;
        stbuf->st_size = sqrawsize;
    }
    else
        res = -ENOENT;
    return res;
    /*
    if(sqfuse_lookup(&fs, &inode, path))
        return -ENOENT;
    if(sqfs_stat(fs, &inode, stbuf))
        return -ENOENT;
    */
    /*
	(void) fi;
	int res = 0;

	//} else if (strcmp(path+1, options.filename) == 0) {
	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
	} else if(strcmp(path, sqrawpath) == 0) {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = sqrawsize;
		//stbuf->st_size = strlen(options.mntpt);
	} else
		res = -ENOENT;

	return res;
    */
    //return 0;
};

static int sqfuse_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags)
{
    qDebug() << "sqfuse_readdir() run";
    /*
    sqfs_err err;
    sqfs* fs;
    sqfs_inode* inode;
    sqfs_dir dir;
    sqfs_name namebuf;
    sqfs_dir_entry entry;
    struct stat st;
    sqfuse_lookup(&fs, NULL, NULL);
    inode = (sqfs_inode*)(intptr_t)fi->fh;
    if(sqfs_dir_open(fs, inode, &dir, offset))
        return -EINVAL;

    memset(&st, 0, sizeof(st));
    sqfs_dentry_init(&entry, namebuf);
    while(sqfs_dir_next(fs, &dir, &entry, &err))
    {
        sqfs_off_t doff = sqfs_dentry_next_offset(&entry);
        st.st_mode = sqfs_dentry_mode(&entry);
        if(filler(buf, sqfs_dentry_name(&entry), &st, doff, (fuse_fill_dir_flags)0))
            return 0;
    }
    if(err)
    {
        qDebug() << "readdir error";
        return -EIO;
    }
    return 0;
    */
    if(strcmp(path, "/") != 0)
        return -ENOENT;
    filler(buf, sqrawpath + 1, NULL, 0, (fuse_fill_dir_flags)0);
    return 0;
    /*
	(void) offset;
	(void) fi;
	(void) flags;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0, (fuse_fill_dir_flags)0);
	filler(buf, "..", NULL, 0, (fuse_fill_dir_flags)0);
	filler(buf, sqrawpath + 1, NULL, 0, (fuse_fill_dir_flags)0);

	return 0;
    */
};

static int sqfuse_open(const char *path, struct fuse_file_info *fi)
{
    qDebug() << "sqfuse_open() run";
    /*
    squishfs* sqsh;
    sqsh = malloc(size(*sqsh));
    if(!sqsh)
        perror("Can't allocate memory");
    else
    {
        memset(sqsh, 0, sizeof(*sqsh));
        if(sqfs_open_image(&sqsh->fs, path, offset) == SQFS_OK)
        {
            if(sqfs_inode_get(&sqsh->fs, &sqsh->root, sqfs_inode_root(&sqsh->fs)))
                fprintf(stderr, "Can't find the root of the filesystem!\n");
            else
                return sqsh;
            sqfs_destroy(&sqsh->fs);
        }
        free(sqsh);
    }
    return NULL;
    */
    /*
    sqfs* fs;
    sqfs_inode* inode;
    if(fi->flags & (O_WRONLY | O_RDWR))
        return -EROFS;
    inode = (sqfs_inode*)malloc(sizeof(*inode));
    if(!inode)
        return -ENOMEM;
    if(sqfuse_lookup(&fs, inode, path))
    {
        free(inode);
        return -ENOENT;
    }
    if(!S_ISREG(inode->base.mode))
    {
        free(inode);
        return -EISDIR;
    }
    fi->fh = (intptr_t)inode;
    fi->keep_cache = 1;
    return 0;
    */
    if(strcmp(path, sqrawpath) != 0)
        return -ENOENT;
    if((fi->flags & O_ACCMODE) != O_RDONLY)
        return -EACCES;
    return 0;
    /*
	if(strcmp(path, sqrawpath) != 0)
		return -ENOENT;

	if ((fi->flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

	return 0;
    */
};

static int sqfuse_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi)
{
    /*
    qDebug() << "sqfuse_read() run";
    sqfs* fs;
    squishfs* squisher = (squishfs*)(fuse_get_context()->private_data);
    fs = &squisher->fs;
    //sqfuse_lookup(&fs, NULL, NULL);
    //squishfs* squisher = (squishfs*)(fuse_get_context()->private_data);
    //*fs = &squisher->fs;
    sqfs_inode* inode = (sqfs_inode*)(intptr_t)fi->fh;

    off_t osize = size;
    if(sqfs_read_range(fs, inode, offset, &osize, buf))
    {
        qDebug() << "read error";
        return -EIO;
    }
    return osize;
    */
    /*
    ssize_t res = 0;
    res = squash_lseek(sqvfd, offset, SQUASH_SEEK_SET);
    qDebug() << "lseek return code:" << res;
    res = squash_read(sqvfd, buf, offset);
    qDebug() << "read return code:" << res;
    */

    //ssize_t squash_read(int vfd, void *buf, sqfs_off_t nbyte);
    /*
	int res = 0;
	(void) fi;
	if(strcmp(path, sqrawpath) != 0)
		return -ENOENT;
	//res = squash_lseek(sqvfd, offset, SQUASH_SEEK_SET);
	//off_t squash_lseek(int vfd, off_t offset, int whence);
	//af_seek(afimage, (uint64_t)offset, SEEK_SET);
        //off64_t libewf_handle_seek_offset(libewf_handle_t *handle, off64_t offset, int whence, libewf_error_t **error );
        //res = libewf_handle_seek_offset(ewfhandle, offset, SEEK_SET, &ewferror);

	errno = 0;
	//res = squash_read(sqvfd, buf, offset);
	//ssize_t squash_read(int vfd, void *buf, sqfs_off_t nbyte);
	//res = af_read(afimage, (unsigned char*)buf, (int)size);
        //res = libewf_handle_read_buffer(ewfhandle, buf, size, &ewferror);
        //ssize_t libewf_handle_read_buffer(libewf_handle_t *handle, void *buffer, size_t buffer_size, libewf_error_t **error);

	return res;
    */
};

static void sqfuse_destroy(void* param)
{
    qDebug() << "sqfuse_destroy() run";
    //squishfs* sqsh = (squishfs*)param;
    //sqfs_destroy(&sqsh->fs);
    //free(sqsh);
    //squash_close(sqvfd);
    //libewf_handle_close(ewfhandle, &ewferror);
    //libewf_handle_free(&ewfhandle, &ewferror);
    //af_close(afimage);
    XFREE(sqrawpath);
    return;
};

/*
static int sqfuse_opendir(const char* path, struct fuse_file_info* fi)
{
    qDebug() << "sqfuse_opendir() run";
    sqfs* fs;
    sqfs_inode* inode;
    inode = (sqfs_inode*)malloc(sizeof(*inode));
    if(!inode)
        return -ENOMEM;
    if(sqfuse_lookup(&fs, inode, path))
    {
        free(inode);
        return -ENOENT;
    }
    if(!S_ISDIR(inode->base.mode))
    {
        free(inode);
        return -ENOTDIR;
    }
    fi->fh = (intptr_t)inode;
    return 0;
};

static int sqfuse_releasedir(const char* path, struct fuse_file_info* fi)
{
    qDebug() << "sqfuse_releasedir() run";
    free((sqfs_inode*)(intptr_t)fi->fh);
    fi->fh = 0;
    return 0;
};

static int sqfuse_release(const char* path, struct fuse_file_info* fi)
{
    qDebug() << "sqfuse_release() run";
    free((sqfs_inode*)(intptr_t)fi->fh);
    fi->fh = 0;
    return 0;
};

static int sqfuse_readlink(const char* path, char* buf, size_t size)
{
    qDebug() << "sqfuse_readlink() run";
    sqfs* fs;
    sqfs_inode inode;
    if(sqfuse_lookup(&fs, &inode, path))
        return -ENOENT;
    if(!S_ISLNK(inode.base.mode))
        return -EINVAL;
    else if(sqfs_readlink(fs, &inode, buf, &size))
    {
        qDebug() << "readlink error";
        return -EIO;
    }
    return 0;
};

static int sqfuse_listxattr(const char* path, char* buf, size_t size)
{
    qDebug() << "sqfuse_listxattr() run";
    sqfs* fs;
    sqfs_inode inode;
    int ferr;
    if(sqfuse_lookup(&fs, &inode, path))
        return -ENOENT;
    ferr = sqfs_listxattr(fs, &inode, buf, &size);
    if(ferr)
        return -ferr;

    return size;
};

static int sqfuse_getxattr(const char* path, const char* name, char* value, size_t size
#ifdef FUSE_XATTR_POSITION
	, uint32_t position
#endif
	)
{
    qDebug() << "sqfuse_getxattr() run";
    sqfs* fs;
    sqfs_inode inode;
    size_t real = size;
#ifdef FUSE_XATTR_POSITION
    if(position != 0)
        return -EINVAL;
#endif
    if(sqfuse_lookup(&fs, &inode, path))
        return -ENOENT;
    if((sqfs_xattr_lookup(fs, &inode, name, value, &real)))
    {
        qDebug() << "getxattr error";
        return -EIO;
    }
    if(real == 0)
        return -sqfs_enoattr();
    if(size != 0 && size < real)
        return -ERANGE;
    return real;
};

static int sqfuse_statfs(const char* path, struct statvfs* st)
{
    qDebug() << "sqfuse_statfs run()";
    squishfs* sqsh = (squishfs*)fuse_get_context()->private_data;
    return sqfs_statfs(&sqsh->fs, st);
};

static int sqfuse_create(const char* unused_path, mode_t unused_mode, struct fuse_file_info* unused_fi)
{
    return -EROFS;
};
*/
static const struct fuse_operations sqfuse_oper = {
	.getattr	= sqfuse_getattr,
        //.readlink       = sqfuse_readlink,
        //.mknod          = sqfuse_mknod,
        //.mkdir          = sqfuse_mkdir,
        //.unlink         = sqfuse_unlink,
        //.rmdir          = sqfuse_rmdir,
        //.symlink        = sqfuse_symlink,
        //.rename         = sqfuse_rename,
        //.link           = sqfuse_link,
        //.chmod          = sqfuse_chmod,
        //.chown          = sqfuse_chown,
        //.truncate       = sqfuse_truncate,*/
	.open		= sqfuse_open,
	.read		= sqfuse_read,
        //.write          = sqfuse_write,
        //.statfs         = sqfuse_statfs,
        //.flush          = sqfuse_flush,
        //.release        = sqfuse_release,
        //.fsync          = sqfuse_fsync,
        //.setxattr       = sqfuse_setxattr,
        //.getxattr       = sqfuse_getxattr,
        //.listxattr      = sqfuse_listxattr,
        //.removexattr    = sqfuse_removexattr,
        //.opendir        = sqfuse_opendir,
	.readdir	= sqfuse_readdir,
        //.releasedir     = sqfuse_releasedir,
        //.fsyncdir       = sqfuse_fsyncdir,
	.init           = sqfuse_init,
	.destroy	= sqfuse_destroy,
        //.access         = sqfuse_access,
        //.create         = sqfuse_create,
        //.lock           = sqfuse_lock,
};

/*
static squishfs* squish_open(const char* path, size_t offset)
{
    squishfs* sqsh;
    sqsh = (squishfs*)malloc(sizeof(*sqsh));
    if(!sqsh)
        perror("Can't allocate memory");
    else
    {
        memset(sqsh, 0, sizeof(*sqsh));
        if(sqfs_open_image(&sqsh->fs, path, offset) == SQFS_OK)
        {
            if(sqfs_inode_get(&sqsh->fs, &sqsh->root, sqfs_inode_root(&sqsh->fs)))
                fprintf(stderr, "Can't find the root of the filesystem!\n");
            else
                return sqsh;
            sqfs_destroy(&sqsh->fs);
        }
        free(sqsh);
    }
    return NULL;
};
*/

void* sqfuselooper(void *data)
{
    struct fuse* fuse = (struct fuse*) data;
    fuse_loop(fuse);
    //int ret = fuse_loop(fuse);
    //printf("fuse loop return: %d\n", ret);
    //pthread_exit(NULL);
};

struct fuse_args sqargs;
struct fuse* sqfuser = NULL;
//struct fuse_session* affusersession;
pthread_t sqfusethread;

void SquashFuser(QString imgpath, QString imgfile)
{
    //extern const uint8_t libsquash_fixture[];
    //sqfs_err sqerr;
    //qDebug() << "imgfile:" << imgfile;
    //const uint8_t* sqfsmem = ReadFileBytes(imgfile.toStdString().c_str());
    //uint8_t* sqfsmem = ReadFileBytes(imgfile.toStdString().c_str());
    //squash_start();
    //sqfs* fs;
    //sqfs_fd_t* sqfd = imgfile.toStdString().c_str();
    //fs = (sqfs*)calloc(sizeof(sqfs), 1);sqfs_fd_ope
    //sqerr = sqfs_init(fs, sqfd, 0);
    //sqerr = sqfs_open_image(fs, sqfsmem, 0);


    //sqfs_open_image(fs, (uint8_t*)sqfsmem, 0);
    //enclose_io_fs = (sqfs *)calloc(sizeof(sqfs), 1);
    //sqfs_open_image(enclose_io_fs, libsquash_fixture, 0);
    /*
    squishfs* squish;
    squish = squish_open(imgfile.toStdString().c_str(), 0);
    if(squish == NULL)
        qDebug() << "squish open fails.";
    else
    {
        qDebug() << "squish open works:";
        qDebug() << "squish inode:" << squish->root.xtra.dir.offset << squish->root.xtra.dir.dir_size << squish->root.xtra.dir.start_block;
    }
    */
    //char** ewffilenames = NULL;
    //char* filenames[1] = {NULL};
    //char* filenames[];
    //system_character_t * const *imgfilenames = NULL;
    //system_character_t **libewf_filenames = NULL;
    //system_character_t *filenames[ 1 ]    = { NULL };
    //filenames[ 0 ]      = (system_character_t *) filename;
    QString filebase = imgfile.split(".").at(0);
    qDebug() << "orig basename:" << filebase;
    char* imgfilesub = NULL;
    imgfilesub = new char[filebase.toStdString().size() + 1];
    strcpy(imgfilesub, filebase.toStdString().c_str());
    int ret;
    char* afpath = NULL;
    char* afbasename = NULL;
    size_t rawpathlen = 0;
    //char* rootpath = "squashfs-root/";
    char** fargv = NULL;
    fargv = XCALLOC(char *, 3);
    int fargc = 0;
    char* ipath = new char[imgpath.toStdString().size() + 1];
    strcpy(ipath, imgpath.toStdString().c_str());
    char* iname = new char[imgfile.toStdString().size() + 1];
    strcpy(iname, imgfile.toStdString().c_str());



    //filenames[0] = (char*)iname;
    //libewf_handle_initialize(&ewfhandle, &ewferror);
    //ibewf_handle_open(ewfhandle, filenames, 1, LIBEWF_OPEN_READ, &ewferror);
    //libewf_handle_open(ewfhandle, (char* const)iname, 1, LIBEWF_OPEN_READ, &ewferror)
    //int squash_open(sqfs *fs, const char *path);
    fargv[0] = "./sqfuse";
    //fargv[1] = "-s";
    //fargv[2] = "auto_unmount";
    fargc = 1;
    //for(int i=0; i < fargc; i++)
    //    printf("fargv[%d]: %s\n", i, fargv[i]);

    //afimage = af_open(iname, O_RDONLY|O_EXCL,0);
    afpath = xstrdup(ipath);
    printf("efpath: %s\n", afpath);
    afbasename = basename(imgfilesub);
    //afbasename = basename(iname);
    printf("afbasename: %s\n", afbasename);
    rawpathlen = 1 + strlen(afbasename) + strlen(sqrawext) + 1;
    //rawpathlen = 1 + strlen(rootpath) + strlen(afbasename) + strlen(sqrawext) + 1;
    sqrawpath = XCALLOC(char, rawpathlen);
    sqrawpath[0] = '/';
    //strcat(sqrawpath, rootpath);
    strcat(sqrawpath, afbasename);
    strcat(sqrawpath, sqrawext);
    sqrawpath[rawpathlen - 1] = 0;
    qDebug() << "rawpath:" << QString::fromStdString(std::string(sqrawpath));
    printf("rawpath: %s", sqrawpath);
    XFREE(afpath);

    // CURRENT ATTEMPT WILL BE TO USE SQUASHFS_FS.H AND SIMPLY READ FILE INTO A FD = OPEN(), AS IN UNSQUASHFS.C
    // THEN READ IN THE SUPER BLOCK... AS IN READ_SUPER()....
    // THEN MAYBE I'LL CONTINUE TRYING LIBSQUASH, BUT IT WON'T LOAD SQUASHFS PROPERLY...

    long long bytes = sizeof(struct squashfs_super_block);
    int fd = open(imgfile.toStdString().c_str(), O_RDONLY);
    struct squashfs_super_block superblock;
    int res = 0;
    int count = 0;
    lseek(fd, 0, SEEK_SET);
    for(count = 0; count < bytes; count += res)
    {
        res = read(fd, &superblock + count, bytes - count);
    }
    qDebug() << "superblock inode cnt:" << superblock.inodes;
    qDebug() << "superblock root inode:" << superblock.root_inode;
    qDebug() << "superblock root start_block:" << (superblock.root_inode >> 16);
    qDebug() << "superblock root offset:" << (superblock.root_inode & 0xffff);
    sqfs* fs;
    /*
    sqerr = sqfs_init(fs, (sqfs_fd_t)fd, 0);
    if(sqerr == SQFS_OK)
    {
        qDebug() << "it works";
    }
    */
    //err = sqfs_init(fs, fd, offset);
    //squashfs_operations *s_ops;
    //struct inode* rootinode = 
    //qDebug() << "root inode offset:" << superblock.root_inode;
    //i = s_ops->read_inode(start_block, offset);
    //print_filename(pathname, i);

    /*
    if(sqerr == SQFS_OK)
    {
	sqvfd = squash_open(fs, sqrawpath);
	qDebug() << "it opened sqfs correctly";
    }
    else
	qDebug() << "it failed to open sqfs correctly";
    */


    //int libewf_handle_get_media_size(libewf_handle_t *handle, size64_t *media_size, libewf_error_t **error );
    //libewf_handle_get_media_size(ewfhandle, (size64_t*)&erawsize, &ewferror);

    //rawsize = af_get_imagesize(afimage);
    
    ///*
    //sqvfd = sqfs_open_image(squish, (const uint8_t*)iname, 0);
    //qDebug() << "sqvfd:" << sqvfd;
    //sqvfd = squash_open(squish, imgfile.toStdString().c_str());
    struct fuse_loop_config config;
    config.clone_fd = 0;
    config.max_idle_threads = 5;
    sqargs = FUSE_ARGS_INIT(fargc, fargv);
    sqfuser = fuse_new(&sqargs, &sqfuse_oper, sizeof(fuse_operations), NULL);
    ret = fuse_mount(sqfuser, imgpath.toStdString().c_str());
    fuse_daemonize(1);
    pthread_create(&sqfusethread, NULL, sqfuselooper, (void*) sqfuser);
    //*/
    //affuser = fuse_new(&args, &affuse_oper, sizeof(fuse_operations), NULL);
    //if(affuser == NULL)
    //    qDebug() << "affuser new error.";
    //ret = fuse_mount(affuser, imgpath.toStdString().c_str());
    //int retd = fuse_daemonize(1);

    //affusersession = fuse_get_session(affuser); 
    //struct fuse_session* se = fuse_get_session(affuser);
    //int retsh = fuse_set_signal_handlers(se);
    //qDebug() << "fuse session signal handlers:" << retsh;
    //pthread_t threadId;
    //int perr = pthread_create(&fusethread, NULL, fuselooper, (void *) affuser);
};

#endif // SQFUSE_H
