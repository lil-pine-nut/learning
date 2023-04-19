
#include <unistd.h>   /* chown() */
#include <sys/stat.h> /* stat(), chmod() */
#include <utime.h>    /* utime() */

#ifndef __SET__ATTRIBUTEES__SS__H__
#define __SET__ATTRIBUTEES__SS__H__

/* Copy file attributes, from -> to, as best we can.  This is best effort, so
   no errors are reported.  The mode bits, including suid, sgid, and the sticky
   bit are copied (if allowed), the owner's user id and group id are copied
   (again if allowed), and the access and modify times are copied. */
/* 尽可能地复制文件属性，从 -> 到。 这是最大的努力，所以
   不报告任何错误。 模式位，包括suid、sgid，和sticky
   位被复制（如果允许的话），所有者的用户ID和组ID被复制（同样如果允许的话），访问权限被复制。
   (如果允许的话），访问和修改时间也被复制。*/
void copymeta(const char *from, const char *to)
{
    struct stat was;
    struct utimbuf when;

    /* get all of from's Unix meta data, return if not a regular file */
    if (stat(from, &was) != 0 || (was.st_mode & S_IFMT) != S_IFREG)
        return;

    /* set to's mode bits, ignore errors */
    (void)chmod(to, was.st_mode & 07777);

    /* copy owner's user and group, ignore errors */
    (void)chown(to, was.st_uid, was.st_gid);

    /* copy access and modify times, ignore errors */
    when.actime = was.st_atime;
    when.modtime = was.st_mtime;
    (void)utime(to, &when);
}

#endif