/***
*fseeki64.c - reposition file pointer on a stream
*
*       Copyright (c) 1994-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       defines _fseeki64() - move the file pointer to new place in file
*
*******************************************************************************/

#include <cruntime.h>
#include <stdio.h>
#include <file2.h>
#include <dbgint.h>
#include <msdos.h>
#include <errno.h>
#include <malloc.h>
#include <io.h>
#include <stddef.h>
#include <internal.h>
#include <mtdll.h>

/***
*int _fseeki64(stream, offset, whence) - reposition file pointer
*
*Purpose:
*
*       Reposition file pointer to the desired location.  The new location
*       is calculated as follows:
*                                { whence=0, beginning of file }
*               <offset> bytes + { whence=1, current position  }
*                                { whence=2, end of file       }
*
*       Be careful to coordinate with buffering.
*
*Entry:
*       FILE *stream  - file to reposition file pointer on
*       _int64 offset - offset to seek to
*       int whence    - origin offset is measured from (0=beg, 1=current pos,
*                       2=end)
*
*Exit:
*       returns 0 if succeeds
*       returns -1 and sets errno if fails
*       fields of FILE struct will be changed
*
*Exceptions:
*
*******************************************************************************/

#ifdef _MT

int __cdecl _fseeki64 (
        FILE *stream,
        __int64 offset,
        int whence
        )
{
        int retval;

        _ASSERTE(stream != NULL);

        _lock_str(stream);

        __try {
                retval = _fseeki64_lk (stream, offset, whence);
        }
        __finally {
                _unlock_str(stream);
        }

        return(retval);
}


/***
*_fseeki64_lk() - Core _fseeki64() routine (stream is locked)
*
*Purpose:
*       Core _fseeki64() routine; assumes that caller has the stream locked.
*
*Entry:
*
*Exit:
*
*Exceptions:
*
*******************************************************************************/

int __cdecl _fseeki64_lk (

#else  /* _MT */

int __cdecl _fseeki64 (

#endif  /* _MT */

        FILE *str,
        __int64 offset,
        int whence
        )
{


        REG1 FILE *stream;

        _ASSERTE(str != NULL);

        /* Init stream pointer */
        stream = str;

        if ( !inuse(stream) || ((whence != SEEK_SET) && (whence != SEEK_CUR) &&
            (whence != SEEK_END)) ) {
                errno=EINVAL;
                return(-1);
        }

        /* Clear EOF flag */

        stream->_flag &= ~_IOEOF;

        /* If seeking relative to current location, then convert to
           a seek relative to beginning of file.  This accounts for
           buffering, etc. by letting fseek() tell us where we are. */

        if (whence == SEEK_CUR) {
                offset += _ftelli64_lk(stream);
                whence = SEEK_SET;
        }

        /* Flush buffer as necessary */

        _flush(stream);

        /* If file opened for read/write, clear flags since we don't know
           what the user is going to do next. If the file was opened for
           read access only, decrease _bufsiz so that the next _filbuf
           won't cost quite so much */

        if (stream->_flag & _IORW)
                stream->_flag &= ~(_IOWRT|_IOREAD);
        else if ( (stream->_flag & _IOREAD) && (stream->_flag & _IOMYBUF) &&
                  !(stream->_flag & _IOSETVBUF) )
                stream->_bufsiz = _SMALL_BUFSIZ;

        /* Seek to the desired locale and return. */

        return(_lseeki64(_fileno(stream), offset, whence) == -1i64 ? -1 : 0);
}




*******************************************************************************/

__int64 __cdecl _ftelli64_lk (

#else  /* _MT */

__int64 __cdecl _ftelli64 (

#endif  /* _MT */

        FILE *str
        )
{
        REG1 FILE *stream;
        size_t offset;
        __int64 filepos;
        REG2 char *p;
        char *max;
        int fd;
        size_t rdcnt;

        _ASSERTE(str != NULL);

        /* Init stream pointer and file descriptor */
        stream = str;
        fd = _fileno(stream);

        if (stream->_cnt < 0)
                stream->_cnt = 0;

        if ((filepos = _lseeki64(fd, 0i64, SEEK_CUR)) < 0L)
                return(-1i64);

        if (!bigbuf(stream))            /* _IONBF or no buffering designated */
                return(filepos - stream->_cnt);

        offset = (size_t)(stream->_ptr - stream->_base);

        if (stream->_flag & (_IOWRT|_IOREAD)) {
                if (_osfile(fd) & FTEXT)
                        for (p = stream->_base; p < stream->_ptr; p++)
                                if (*p == '\n')  /* adjust for '\r' */
                                        offset++;
        }
        else if (!(stream->_flag & _IORW)) {
                errno=EINVAL;
                return(-1i64);
        }

        if (filepos == 0i64)
                return((__int64)offset);

        if (stream->_flag & _IOREAD)    /* go to preceding sector */

                if (stream->_cnt == 0)  /* filepos holds correct location */
                        offset = 0;

                else {

                        /* Subtract out the number of unread bytes left in the
                           buffer. [We can't simply use _iob[]._bufsiz because
                           the last read may have hit EOF and, thus, the buffer
                           was not completely filled.] */

                        rdcnt = stream->_cnt + (size_t)(stream->_ptr - stream->_base);

                        /* If text mode, adjust for the cr/lf substitution. If
                           binary mode, we're outta here. */
                        if (_osfile(fd) & FTEXT) {
                                /* (1) If we're not at eof, simply copy _bufsiz
                                   onto rdcnt to get the # of untranslated
                                   chars read. (2) If we're at eof, we must
                                   look through the buffer expanding the '\n'
                                   chars one at a time. */

                                /* [NOTE: Performance issue -- it is faster to
                                   do the two _lseek() calls than to blindly go
                                   through and expand the '\n' chars regardless
                                   of whether we're at eof or not.] */

                                if (_lseeki64(fd, 0i64, SEEK_END) == filepos) {

                                        max = stream->_base + rdcnt;
                                        for (p = stream->_base; p < max; p++)
                                                if (*p == '\n')
                                                        /* adjust for '\r' */
                                                        rdcnt++;

                                        /* If last byte was ^Z, the lowio read
                                           didn't tell us about it.  Check flag
                                           and bump count, if necessary. */

                                        if (stream->_flag & _IOCTRLZ)
                                                ++rdcnt;
                                }

                                else {

                                        _lseeki64(fd, filepos, SEEK_SET);

                                        /* We want to set rdcnt to the number
                                           of bytes originally read into the
                                           stream buffer (before crlf->lf
                                           translation). In most cases, this
                                           will just be _bufsiz. However, the
                                           buffer size may have been changed,
                                           due to fseek optimization, at the
                                           END of the last _filbuf call. */

                                        if ( (rdcnt <= _SMALL_BUFSIZ) &&
                                             (stream->_flag & _IOMYBUF) &&
                                             !(stream->_flag & _IOSETVBUF) )
                                        {
                                                /* The translated contents of
                                                   the buffer is small and we
                                                   are not at eof. The buffer
                                                   size must have been set to
                                                   _SMALL_BUFSIZ during the
                                                   last _filbuf call. */

                                                rdcnt = _SMALL_BUFSIZ;
                                        }
                                        else
                                                rdcnt = stream->_bufsiz;


                                        /* If first byte in untranslated buffer
                                           was a '\n', assume it was preceeded
                                           by a '\r' which was discarded by the
                                           previous read operation and count
                                           the '\n'. */
                                        if  (_osfile(fd) & FCRLF)
                                                ++rdcnt;
                                }

                        } /* end if FTEXT */

                        filepos -= (__int64)rdcnt;

                } /* end else stream->_cnt != 0 */

        return(filepos + (__int64)offset);
}
