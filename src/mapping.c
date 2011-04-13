#include "mapping.h"

static mode_pair mode_map_filetype[] = {
    { RHI_FILETYPE_DIR,     S_IFDIR },
    { RHI_FILETYPE_CHR,     S_IFCHR },
    { RHI_FILETYPE_BLK,     S_IFBLK },
    { RHI_FILETYPE_REG,     S_IFREG },
    { RHI_FILETYPE_IFO,     S_IFIFO },
    { RHI_FILETYPE_LNK,     S_IFLNK },
    { RHI_FILETYPE_SOCK,    S_IFSOCK }
};

static mode_pair mode_map_perm[] = {
    { RHI_PERM_RUSR,        S_IRUSR },
    { RHI_PERM_WUSR,        S_IWUSR },
    { RHI_PERM_XUSR,        S_IXUSR },
    { RHI_PERM_RGRP,        S_IRGRP },
    { RHI_PERM_WGRP,        S_IWGRP },
    { RHI_PERM_XGRP,        S_IXGRP },
    { RHI_PERM_ROTH,        S_IROTH },
    { RHI_PERM_WOTH,        S_IWOTH },
    { RHI_PERM_XOTH,        S_IXOTH }
};

#define mode_map_len(mm) (sizeof(mm)/sizeof(mode_pair))

static errno_pair errno_map[] = {
    // TODO
};
#define errno_map_len(em) (sizeof(em)/sizeof(errno_pair))

unsigned int
mapping_mode_l2p(mode_t mode)
{
    unsigned int md = 0;
    unsigned int i = 0;

    for (i=0; i<mode_map_len(mode_map_filetype); ++i) {
        if (mode_map_filetype[i].local & mode) {
            md = mode_map_filetype[i].protocol;
        }
    }

    if (md == 0) {
        md = RHI_FILETYPE_REG; // fallback to regular file
    }

    for (i=0; i<mode_map_len(mode_map_perm); ++i) {
        if (mode_map_perm[i].local & mode) {
            md |= mode_map_perm[i].protocol;
        }
    }
    return md;
};


mode_t
mapping_mode_p2l(unsigned int md)
{
    mode_t mode = 0;
    unsigned int i = 0;

    for (i=0; i<mode_map_len(mode_map_filetype); ++i) {
        if (mode_map_filetype[i].protocol & md) {
            mode = mode_map_filetype[i].local;
        }
    }

    if (mode == 0) {
        mode = S_IFREG; // fallback to regular file
    }

    for (i=0; i<mode_map_len(mode_map_perm); ++i) {
        if (mode_map_perm[i].protocol & md) {
            mode |= mode_map_perm[i].local;
        }
    }
    return mode;
};


int
mapping_errno_l2p(int lerrno) 
{
    int perrno = RHIZOFS__ERRNO_TYPE__ERRNO_UNKNOWN; // default value
    unsigned int i=0;

    for (i=0; i<errno_map_len(errno_map); ++i) {
        if (errno_map[i].local == lerrno) {
            perrno = errno_map[i].protocol;
            break;
        }
    }

    return perrno;
}


int mapping_errno_p2l(int perrno)
{
    int lerrno = EIO; // default value
    unsigned int i=0;

    for (i=0; i<errno_map_len(errno_map); ++i) {
        if (errno_map[i].protocol == perrno) {
            lerrno = errno_map[i].local;
            break;
        }
    }

    return lerrno;
}

