#include "fs.h"

extern size_t ramdisk_read(void *, size_t, size_t);
extern size_t ramdisk_write(const void*, size_t, size_t);

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, invalid_write},
  {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

int fs_open(const char *path){
  for (int i = 0; i < NR_FILES; ++i)
  {
    if (strcmp(path, file_table[i].name) == 0)
    {
      return i;
    }
  }
  printf("%s, no such file\n", path);
  assert(0);
  return -1;
}

int fs_close(int fd){
  file_table[fd].open_offset = 0;
  return 0;
}

// size_t fs_read(int fd, void *buf, size_t length){
//   if (fd < 0 || fd >= NR_FILES)
//   {
//     printf("Wrong fd: %d\n", fd);
//     assert(0);
//   }

//   size_t temp = file_table[fd].open_offset;
//   if (file_table[fd].read == NULL)
//   {
//     if (length <= file_table[fd].size)
//     {
//       temp = temp + length;
//     }else{
//       temp = file_table[fd].size;
//     }
//     temp = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, temp);
//     file_table[fd].open_offset = file_table[fd].open_offset + temp;
//     return temp;
//   }else{
//     temp = length;
//     if (file_table[fd].size && file_table[fd].open_offset + length > file_table[fd].size)
//     {
//       temp = file_table[fd].size - file_table[fd].open_offset;
//     }

//     temp = file_table[fd].read(buf, file_table[fd].open_offset, temp);
//     file_table[fd].open_offset = file_table[fd].open_offset + temp;
//     return temp;
//   }

// }

static inline void fd_check(int fd) {
  assert(0 <= fd && fd < NR_FILES);
}

size_t fs_read(int fd, void *buf, size_t len){
  fd_check(fd);
  size_t sz;
  if (file_table[fd].read == NULL) {
    sz = file_table[fd].open_offset + len <= file_table[fd].size ? len : file_table[fd].size - file_table[fd].open_offset;
    sz = ramdisk_read(buf, file_table[fd].disk_offset + file_table[fd].open_offset, sz);
    file_table[fd].open_offset += sz;
    return sz;
  } else {
    sz = len;
    if (file_table[fd].size && file_table[fd].open_offset + len > file_table[fd].size) {
      sz = file_table[fd].size - file_table[fd].open_offset;
    }
    sz = file_table[fd].read(buf, file_table[fd].open_offset, sz);
    file_table[fd].open_offset += sz;
    return sz;
  }
}

size_t fs_lseek(int fd, size_t offset, int pl){
  if (fd < 0 || fd >= NR_FILES)
  {
    printf("Wrong fd: %d\n", fd);
    assert(0);
  }

  switch(pl){

  }
}

void init_fs() {
  // TODO: initialize the size of /dev/fb
}
