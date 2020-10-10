#include <mntent.h>
#include <sys/vfs.h>
#include <fnmatch.h>
#include <jansson.h>

#define IDE_SCSI_USB_DEV "/dev/[sh]d?[0-9]*"
#define SD_CARDS_DEV     "/dev/mmcblk[0-9]*p[0-9]*"
#define NVME_DEV         "/dev/nvme[0-9]*p[0-9]*"

int storage_usage (json_t *storage_state)
{
    FILE* mtab = setmntent (_PATH_MOUNTED, "r");
    struct mntent* m;

    for (; (m = getmntent (mtab));)
    {
        struct statfs fs;
        if ((m->mnt_dir != NULL)                  &&
                (statfs (m->mnt_dir, &fs) == 0)   &&
                (hasmntopt(m, MNTOPT_RW) != NULL) &&
                (!fnmatch (IDE_SCSI_USB_DEV, m->mnt_fsname, 0) ||
                 !fnmatch (SD_CARDS_DEV,     m->mnt_fsname, 0) ||
                 !fnmatch (NVME_DEV,         m->mnt_fsname, 0)))
        {

            json_t *device;
            device = json_pack ("{s:s, s:s, s:I, s:I, s:I, s:I}",
                                "Filesystem",     m->mnt_fsname,
                                "Mount Point",    m->mnt_dir,
                                "Total",          fs.f_blocks,
                                "Free",           fs.f_bfree,
                                "Available",      fs.f_bavail,
                                "Block size, kB", fs.f_bsize / 1024);
//                              "Usage, %", (1000 * (size - free) / (size - free + avail) + 5) / 10);
            json_array_append (storage_state, device);
            json_decref (device);
        }
    }
    
    endmntent (mtab);
    return 0;
}
